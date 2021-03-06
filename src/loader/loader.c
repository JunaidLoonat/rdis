#include "loader.h"

#include "elf64.h"
#include "elf32.h"
#include "pe.h"


_loader * loader_create (const char * filename)
{
    _loader * loader;

    loader = (_loader *) elf64_create(filename);
    if (loader != NULL)
        return loader;

    loader = (_loader *) elf32_create(filename);
    if (loader != NULL)
        return loader;

    loader = (_loader *) pe_create(filename);
    if (loader != NULL)
        return loader;

    return NULL;
}


uint64_t loader_entry (_loader * loader)
{
    struct _loader_object_ptr * loader_object_ptr;
    struct _loader_object     * loader_object;

    loader_object_ptr = (struct _loader_object_ptr *) loader;
    loader_object = loader_object_ptr->loader_object;

    return loader_object->entry(loader);
}


struct _graph * loader_graph (_loader * loader, struct _map * memory)
{
    struct _loader_object_ptr * loader_object_ptr;
    struct _loader_object     * loader_object;

    loader_object_ptr = (struct _loader_object_ptr *) loader;
    loader_object = loader_object_ptr->loader_object;

    return loader_object->graph(loader, memory);
}


struct _map * loader_functions (_loader * loader, struct _map * memory)
{
    struct _loader_object_ptr * loader_object_ptr;
    struct _loader_object     * loader_object;

    loader_object_ptr = (struct _loader_object_ptr *) loader;
    loader_object = loader_object_ptr->loader_object;

    return loader_object->functions(loader, memory);
}


struct _map * loader_labels (_loader * loader, struct _map * memory)
{
    struct _loader_object_ptr * loader_object_ptr;
    struct _loader_object     * loader_object;

    loader_object_ptr = (struct _loader_object_ptr *) loader;
    loader_object = loader_object_ptr->loader_object;

    return loader_object->labels(loader, memory);
}


struct _graph * loader_graph_address (_loader *     loader,
                                      struct _map * memory,
                                      uint64_t      address)
{
    struct _loader_object_ptr * loader_object_ptr;
    struct _loader_object     * loader_object;

    loader_object_ptr = (struct _loader_object_ptr *) loader;
    loader_object = loader_object_ptr->loader_object;

    return loader_object->graph_address(loader, memory, address);
}


struct _map * loader_memory_map (_loader * loader)
{
    struct _loader_object_ptr * loader_object_ptr;
    struct _loader_object     * loader_object;

    loader_object_ptr = (struct _loader_object_ptr *) loader;
    loader_object = loader_object_ptr->loader_object;

    return loader_object->memory_map(loader);
}


struct _map * loader_function_address (_loader *     loader,
                                       struct _map * memory,
                                       uint64_t      address)
{
    struct _loader_object_ptr * loader_object_ptr;
    struct _loader_object     * loader_object;

    loader_object_ptr = (struct _loader_object_ptr *) loader;
    loader_object = loader_object_ptr->loader_object;

    return loader_object->function_address(loader, memory, address);
}


struct _label * loader_label_address (_loader *     loader,
                                      struct _map * memory,
                                      uint64_t      address)
{
    struct _loader_object_ptr * loader_object_ptr;
    struct _loader_object     * loader_object;

    loader_object_ptr = (struct _loader_object_ptr *) loader;
    loader_object = loader_object_ptr->loader_object;

    return loader_object->label_address(loader, memory, address);
}


struct _graph * loader_graph_functions (_loader *     loader,
                                        struct _map * memory,
                                        struct _map * functions)
{
    struct _loader_object_ptr * loader_object_ptr;
    struct _loader_object     * loader_object;

    loader_object_ptr = (struct _loader_object_ptr *) loader;
    loader_object = loader_object_ptr->loader_object;

    return loader_object->graph_functions(loader, memory, functions);
}


struct _map * loader_labels_functions (_loader *     loader,
                                       struct _map * memory,
                                       struct _map * functions)
{
    struct _loader_object_ptr * loader_object_ptr;
    struct _loader_object     * loader_object;

    loader_object_ptr = (struct _loader_object_ptr *) loader;
    loader_object = loader_object_ptr->loader_object;

    return loader_object->labels_functions(loader, memory, functions);
}