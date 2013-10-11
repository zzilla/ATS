/**
 *  module.c
 *
 */

 
#include "osa.h"
#include "class.h"
#include "module.h"

static osa_list_t   mod_list_head = OSA_LIST_HEAD(mod_list_head);


ats_module_t *ats_module_find(const char *name)
{
    osa_assert(name != NULL);

    ats_module_t    *node   = NULL;
    osa_list_t      *l      = NULL;

    for (l = mod_list_head.next; l != &mod_list_head; l = l->next)
    {
        node = osa_list_entry(l, ats_bus_t, list);
        if (!strcmp(node->name, name))
        {
            return node;
        }
    }
    
    return NULL;
}


osa_err_t ats_module_register(ats_module_t *m, const osa_char_t *name, ats_mops_t *ops)
{
    osa_assert(name != NULL);
    
    strncpy(m->name, name, OSA_NAME_MAX-1);
    osa_list_init(&m->list);
    m->ops = ops;
    
    ats_module_t   *p = NULL;

    if ((p = ats_module_find(m->name)) != NULL)
    {
        ats_log_warn("Replace module : name(%s)\n", m->name);
        p = bus;
    }
    else
    {
        ats_log_info("Register new module : name(%s)\n", m->name);
        osa_list_insert_before(&mod_list_head, &m->list);
    }
    
    return OSA_ERR_OK;
}


osa_err_t ATS_ModuleUnregister(ats_module_t *module)
{
    ats_module_t  *p = NULL;

    if ((p = ATS_ModuleFind(module->name)) != NULL)
    {
        p = NULL;
    }

    ats_log_info("Unregister module : name(%s)\n", module->name);
    return OSA_ERR_OK;
}


osa_err_t       ats_moudle_unregister(const osa_char_t *mod_name)
{
    osa_assert(mod_name != NULL);
    
    ats_bus_t *p = NULL;
    
    if ((p = ats_module_find(mod_name)) != NULL)
    {
        osa_list_remove(&p->list);
        ats_log_info("Unregister bus : name(%s)\n", p->name);
    }
    
    return OSA_ERR_OK;
}

void ats_module_init_all(int argc, char **argv)
{
    osa_uint32_t i;
    osa_err_t   err;

    ats_module_t    *node   = NULL;
    osa_list_t      *l      = NULL;

    for (l = mod_list_head.next; l != &mod_list_head; l = l->next)
    {
        node = osa_list_entry(l, ats_module_t, list);
        if (node->ops->parse_conf)
        {
            if (node->ops->parse_conf(node) != OSA_ERR_OK)
            {
                ats_log_error("Failed to initialize module (%s) when parse the configuration file!\n", node->name);
                continue;
            }
        }
        if (node->ops->begin)
        {
            if (node->ops->begin(node, argc, argv) != OSA_ERR_OK)
            {
                ats_log_error("Faieldt to begin module (%s)!\n", node->name);
                continue;
            }
        }
    }
}