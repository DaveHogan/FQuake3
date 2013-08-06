/*
Copyright (c) 2013 William F. Smith

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

#include "m.h"

#include <glib/gprintf.h>
#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/debug-helpers.h>

struct _MDomain {
	MonoDomain *domain;
};

typedef struct {
	MonoAssembly *assembly;
	const gchar *name;
} MAssemblyQuery;


static gchar*
assembly_get_name (MonoAssembly *assembly)
{
	return (gchar *)mono_image_get_name (mono_assembly_get_image (assembly));
}

static void
foreach_assembly (MonoAssembly *assembly, MAssemblyQuery *query)
{
	if (g_strcmp0 (assembly_get_name (assembly), query->name) == 0)
		query->assembly = assembly;
}

static MonoAssembly*
find_assembly (const gchar *name)
{
	MAssemblyQuery query;

	query.name = name;
	mono_assembly_foreach ((GFunc)foreach_assembly, &query);
	return query.assembly;
}


static void
get_method_desc (const gchar *name_space, const gchar *class_name, const gchar *method_name, gchar *name)
{
	g_sprintf (name, "%s.%s:%s", name_space, class_name, method_name);
}

MDomain*
m_domain_new (const gchar *assembly_dir, const gchar *config_dir, const char *root_domain_name, const MRuntime runtime)
{
	MDomain *const domain = (MDomain *)g_malloc0 (sizeof (MDomain));
	gchar *version;

	switch (runtime)
	{
	case M_RUNTIME_4_0:
		version = "v4.0.30319";
		break;
	case M_RUNTIME_4_5:
		version = "v4.0.30319";
		break;
	default:
		g_error ("M: Invalid runtime version.");
	}

	mono_set_dirs (assembly_dir, config_dir);
	domain->domain = mono_jit_init_version (root_domain_name, version);
	return domain;
}


void
m_domain_free (MDomain *const domain)
{
	mono_jit_cleanup (domain->domain);
	g_free (domain);
}


void
m_domain_exec (MDomain *const domain, const gchar *name, gint argc, gchar *argv[])
{
	MonoAssembly *const assembly = mono_domain_assembly_open (domain->domain, name);
	if (!assembly)
		g_error ("M: Unable to load %s assembly.\n", name);

	mono_jit_exec (domain->domain, assembly, argc - 1, argv + 1);
}


void
m_load_assembly (const gchar *name)
{
	MonoAssembly *const assembly = mono_domain_assembly_open (mono_domain_get (), name);
	if (!assembly)
		g_error ("M: Unable to load %s assembly.\n", name);
}


MObject
m_object (const gchar *assembly_name, const gchar *name_space, const gchar *name, gint argc, gpointer *args)
{
	MonoAssembly *const assembly = find_assembly (assembly_name);
	MonoImage *image = image = mono_assembly_get_image (assembly);
	MonoClass *klass = mono_class_from_name (image, name_space, name);
	MonoObject *object = mono_object_new (mono_domain_get (), klass);
	MonoMethod *ctor = mono_class_get_method_from_name (klass, ".ctor", argc);
	MonoType *type = mono_class_get_type (klass);
	
	MObject result;

	if (!ctor)
		g_error ("M: Unable to find constructor for type %s.\n", name);

	if (mono_type_is_struct (type))
	{
		mono_runtime_invoke (ctor, mono_object_unbox (object), args, NULL);
		result.__priv = object;
	}
	else
	{
		mono_runtime_invoke (ctor, object, args, NULL);
		result.__priv = object;
	}

	return result;
}


MObject
m_object_get_property (const MObject object, const gchar *property_name)
{
	MonoClass *klass = mono_object_get_class ((MonoObject *)object.__priv);
	MonoProperty *prop = mono_class_get_property_from_name (klass, property_name);
	MonoType *type = mono_class_get_type (klass);

	MObject result;
	
	if (mono_type_is_struct (type))
	{
		result.__priv = mono_property_get_value (prop, mono_object_unbox ((MonoObject *)object.__priv), NULL, NULL);
	}
	else
	{
		result.__priv = mono_property_get_value (prop, (MonoObject *)object.__priv, NULL, NULL);
	}

	return result;
}


MArray
m_object_get_property_array (const MObject object, const gchar *property_name)
{
	MonoClass *klass = mono_object_get_class ((MonoObject *)object.__priv);
	MonoProperty *prop = mono_class_get_property_from_name (klass, property_name);
	MonoType *type = mono_class_get_type (klass);

	MArray result;
	
	if (mono_type_is_struct (type))
	{
		result.__priv = mono_property_get_value (prop, mono_object_unbox ((MonoObject *)object.__priv), NULL, NULL);
	}
	else
	{
		result.__priv = mono_property_get_value (prop, (MonoObject *)object.__priv, NULL, NULL);
	}

	return result;
}


void
m_object_set_property (const MObject object, const gchar *property_name, gpointer value)
{
	MonoClass *klass = mono_object_get_class ((MonoObject *)object.__priv);
	MonoProperty *prop = mono_class_get_property_from_name (klass, property_name);
	MonoType *type = mono_class_get_type (klass);

	gpointer args [1];

	args [0] = value;
	if (mono_type_is_struct (type))
	{
		mono_property_set_value (prop, mono_object_unbox ((MonoObject *)object.__priv), args, NULL); 
	}
	else
	{
		mono_property_set_value (prop, (MonoObject *)object.__priv, args, NULL);
	}
}


void
m_object_set_field (const MObject object, const gchar *field_name, gpointer value)
{
	MonoClass *klass = mono_object_get_class ((MonoObject *)object.__priv);
	MonoClassField *field = mono_class_get_field_from_name (klass, field_name);
	MonoType *type = mono_class_get_type (klass);

	mono_field_set_value ((MonoObject *)object.__priv, field, value);
}


MObject
m_object_invoke (const MObject object, const gchar *method_name, gint argc, gpointer *args)
{
	MonoClass *klass = mono_object_get_class ((MonoObject *)object.__priv);
	MonoMethod *method = mono_class_get_method_from_name (klass, method_name, argc);
	MonoType *type = mono_class_get_type (klass);

	MObject result;

	if (mono_type_is_struct (type))
	{
		result.__priv = mono_runtime_invoke (method, mono_object_unbox ((MonoObject *)object.__priv), args, NULL);
	}
	else
	{
		result.__priv = mono_runtime_invoke (method, (MonoObject *)object.__priv, args, NULL);
	}

	return result;
}


gpointer
m_object_unbox (const MObject object)
{
	if (!object.__priv)
		g_error ("M: Cannot unbox null object.");

	return mono_object_unbox ((MonoObject *)object.__priv);
}


MObject
m_invoke_method (const gchar *assembly_name, const gchar *name_space, const gchar *static_class_name, const gchar *method_name, void **params)
{
	gchar name[256];

	MonoAssembly *assembly;
	MonoImage *image;
	MonoMethodDesc *method_desc;
	MonoMethod *method;

	MObject result;

	get_method_desc (name_space, static_class_name, method_name, name);

	assembly = find_assembly (assembly_name);

	if (!assembly)
		g_error ("M: Unable to find assembly %s.\n", assembly_name);

	image = mono_assembly_get_image (assembly);
	method_desc = mono_method_desc_new (name, FALSE);
	method = mono_method_desc_search_in_image (method_desc, image);

	mono_method_desc_free (method_desc);

	if (method)
	{
		result.__priv = mono_runtime_invoke (method, NULL, params, NULL);	
		return result;
	}

	g_error ("M: Unable to invoke %s.\n", name);
}


MArray
m_array (const gchar *assembly_name, const gchar *name_space, const gchar *name, const gint size)
{
	MonoAssembly *const assembly = find_assembly (assembly_name);
	MonoImage *image = image = mono_assembly_get_image (assembly);
	MonoClass *klass = mono_class_from_name (image, name_space, name);

	MArray result;

	result.__priv = mono_array_new (mono_domain_get (), klass, size);
	return result;
}

MArray
m_array_int32 (const gint size)
{
	MArray result;

	result.__priv = mono_array_new (mono_domain_get (), mono_get_int32_class (), size);
	return result;
}

gchar*
m_array_addr_with_size (const MArray object, const gint size, const gint index)
{
	mono_array_addr_with_size ((MonoArray *)object.__priv, size, index);
}

gint
m_array_length (const MArray object)
{
	return mono_array_length ((MonoArray *)object.__priv);
}
