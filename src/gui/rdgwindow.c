#include "rdgwindow.h"

#include "instruction.h"
#include "queue.h"

#include <string.h>

struct _rdgwindow * rdgwindow_create (struct _gui * gui, uint64_t top_index)
{
    struct _rdgwindow * rdgwindow;

    rdgwindow = (struct _rdgwindow *) malloc(sizeof(struct _rdgwindow));
    rdgwindow->window         = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    rdgwindow->scrolledWindow = gtk_scrolled_window_new(NULL, NULL);
    rdgwindow->imageEventBox  = gtk_event_box_new();
    rdgwindow->image          = gtk_image_new_from_file("logo.png");
    rdgwindow->menu_popup     = gtk_menu_new();

    rdgwindow->gui            = gui;
    rdgwindow->gui_identifier = gui_add_window(rdgwindow->gui, rdgwindow->window);

    rdgwindow->top_index      = top_index;
    rdgwindow->rdg            = NULL;
    rdgwindow->currently_displayed_graph = NULL;

    rdgwindow->image_drag_x   = 0;
    rdgwindow->image_drag_y   = 0;
    rdgwindow->image_dragging = 0;

    rdgwindow->selected_node  = -1;
    rdgwindow->selected_ins   = -1;
    rdgwindow->node_colors    = NULL;

    rdgwindow->editing        = 0;


    // popup menu stuff
    GtkWidget * menuItem = gtk_menu_item_new_with_label("User Function");
    g_signal_connect(menuItem,
                     "activate",
                     G_CALLBACK(rdgwindow_user_function),
                     rdgwindow);
    gtk_menu_shell_append(GTK_MENU_SHELL(rdgwindow->menu_popup), menuItem);

    gtk_widget_show_all(rdgwindow->menu_popup);




    gtk_container_add(GTK_CONTAINER(rdgwindow->imageEventBox),
                      rdgwindow->scrolledWindow);

    gtk_scrolled_window_add_with_viewport(
        GTK_SCROLLED_WINDOW(rdgwindow->scrolledWindow), rdgwindow->image);

    gtk_container_add(GTK_CONTAINER(rdgwindow->window), rdgwindow->imageEventBox);


    g_signal_connect(rdgwindow->window,
                     "destroy",
                     G_CALLBACK(rdgwindow_destroy_event),
                     rdgwindow);

    g_signal_connect(rdgwindow->imageEventBox,
                     "motion-notify-event",
                     G_CALLBACK(rdgwindow_image_motion_notify_event),
                     rdgwindow);

    g_signal_connect(rdgwindow->imageEventBox,
                     "button-press-event",
                     G_CALLBACK(rdgwindow_image_button_press_event),
                     rdgwindow);

    g_signal_connect(rdgwindow->imageEventBox,
                     "button-release-event",
                     G_CALLBACK(rdgwindow_image_button_release_event),
                     rdgwindow);

    g_signal_connect(rdgwindow->imageEventBox,
                     "key-press-event",
                     G_CALLBACK(rdgwindow_image_key_press_event),
                     rdgwindow);

    g_signal_connect(rdgwindow->scrolledWindow,
                     "size-allocate",
                     G_CALLBACK(rdgwindow_size_allocate_event),
                     rdgwindow);


    gtk_widget_show(rdgwindow->image);
    gtk_widget_show(rdgwindow->scrolledWindow);
    gtk_widget_show(rdgwindow->imageEventBox);


    rdgwindow_graph_update(rdgwindow);

    int width  = rdg_width(rdgwindow->rdg);
    int height = rdg_height(rdgwindow->rdg);

    if (width > RDGWINDOW_MAX_DEFAULT_WIDTH)
        width = RDGWINDOW_MAX_DEFAULT_WIDTH;
    if (height > RDGWINDOW_MAX_DEFAULT_HEIGHT)
        height = RDGWINDOW_MAX_DEFAULT_HEIGHT;

    gtk_window_set_default_size(GTK_WINDOW(rdgwindow->window), width, height);

    rdgwindow->callback_identifier = rdis_add_callback(rdgwindow->gui->rdis,
                                        RDIS_CALLBACK(rdgwindow_rdis_callback),
                                        rdgwindow);

    return rdgwindow;
}


void rdgwindow_delete (struct _rdgwindow * rdgwindow)
{
    if (rdgwindow->rdg != NULL)
        object_delete(rdgwindow->rdg);

    if (rdgwindow->currently_displayed_graph != NULL)
        object_delete(rdgwindow->currently_displayed_graph);

    if (rdgwindow->node_colors != NULL)
        object_delete(rdgwindow->node_colors);

    gui_remove_window(rdgwindow->gui, rdgwindow->gui_identifier);
    rdis_remove_callback(rdgwindow->gui->rdis, rdgwindow->callback_identifier);

    gtk_widget_destroy(rdgwindow->menu_popup);

    //gtk_widget_destroy(rdgwindow->window);

    free(rdgwindow);
}


GtkWidget * rdgwindow_window (struct _rdgwindow * rdgwindow)
{
    return rdgwindow->window;
}


void rdgwindow_image_update (struct _rdgwindow * rdgwindow)
{
    rdg_draw(rdgwindow->rdg);
    GdkPixbuf * pixbuf;
    pixbuf = gdk_pixbuf_get_from_surface(rdgwindow->rdg->surface,
                                         0,
                                         0,
                                         rdg_width(rdgwindow->rdg),
                                         rdg_height(rdgwindow->rdg));
    gtk_image_set_from_pixbuf(GTK_IMAGE(rdgwindow->image), pixbuf);
    g_object_unref(pixbuf);
    while (gtk_events_pending())
        gtk_main_iteration();
}


void rdgwindow_graph_update (struct _rdgwindow * rdgwindow)
{
    // set currently_displayed_graph to top_index's node's family
    if (rdgwindow->currently_displayed_graph != NULL)
        object_delete(rdgwindow->currently_displayed_graph);
    rdgwindow->currently_displayed_graph = graph_family(rdgwindow->gui->rdis->graph,
                                                        rdgwindow->top_index);

    if (rdgwindow->rdg != NULL)
        object_delete(rdgwindow->rdg);

    rdgwindow->rdg = rdg_create(rdgwindow->top_index,
                                rdgwindow->currently_displayed_graph,
                                rdgwindow->gui->rdis->labels);
    rdg_custom_nodes(rdgwindow->rdg,
                     rdgwindow->gui->rdis->graph,
                     rdgwindow->gui->rdis->labels,
                     rdgwindow->node_colors,
                     rdgwindow->selected_ins);
    rdgwindow_image_update(rdgwindow);
}



void rdgwindow_destroy_event (GtkWidget * widget, struct _rdgwindow * rdgwindow)
{
    rdgwindow_delete(rdgwindow);
}



gboolean rdgwindow_image_motion_notify_event (GtkWidget * widget,
                                              GdkEventMotion * event,
                                              struct _rdgwindow * rdgwindow)
{
    if (rdgwindow->image_dragging == 0)
        return TRUE;

    GtkAdjustment * hadjust = gtk_scrolled_window_get_hadjustment(
                          GTK_SCROLLED_WINDOW(rdgwindow->scrolledWindow));
    GtkAdjustment * vadjust = gtk_scrolled_window_get_vadjustment(
                          GTK_SCROLLED_WINDOW(rdgwindow->scrolledWindow));

    double diff_x = rdgwindow->image_drag_x - event->x;
    double diff_y = rdgwindow->image_drag_y - event->y;

    gtk_adjustment_set_value(hadjust, gtk_adjustment_get_value(hadjust) + diff_x);
    gtk_adjustment_set_value(vadjust, gtk_adjustment_get_value(vadjust) + diff_y);

    gtk_scrolled_window_set_hadjustment(
                           GTK_SCROLLED_WINDOW(rdgwindow->scrolledWindow),
                           hadjust);

    gtk_scrolled_window_set_vadjustment(
                           GTK_SCROLLED_WINDOW(rdgwindow->scrolledWindow),
                           vadjust);

    rdgwindow->image_drag_x = event->x;
    rdgwindow->image_drag_y = event->y;

    return FALSE;
}


gboolean rdgwindow_image_button_press_event  (GtkWidget * widget,
                                              GdkEventButton * event,
                                              struct _rdgwindow * rdgwindow)
{
    int x = (int) event->x;
    int y = (int) event->y;

    if ((event->type == GDK_BUTTON_PRESS) && (event->button == 1))
        rdgwindow->image_dragging = 1;

    rdgwindow->image_drag_x = x;
    rdgwindow->image_drag_y = y;

    GtkAdjustment * hadjust = gtk_scrolled_window_get_hadjustment(
                          GTK_SCROLLED_WINDOW(rdgwindow->scrolledWindow));
    GtkAdjustment * vadjust = gtk_scrolled_window_get_vadjustment(
                          GTK_SCROLLED_WINDOW(rdgwindow->scrolledWindow));

    // these are the x,y coords inside the image
    int image_x = x + (int) gtk_adjustment_get_value(hadjust);
    int image_y = y + (int) gtk_adjustment_get_value(vadjust);

    // if the scrolledWindow is larger than the image, we need to adjust for that
    /*
    printf("width: %d %d\n",
           rdgwindow->scrolledWindow_width,
           rdg_width(rdgwindow->rdg));
    printf("height: %d %d\n",
           rdgwindow->scrolledWindow_height,
           rdg_height(rdgwindow->rdg));
    */

    if (rdgwindow->scrolledWindow_width > rdg_width(rdgwindow->rdg))
        image_x -= (rdgwindow->scrolledWindow_width
                    - rdg_width(rdgwindow->rdg)) / 2;

    if (rdgwindow->scrolledWindow_height > rdg_height(rdgwindow->rdg))
        image_y -= (rdgwindow->scrolledWindow_height
                    - rdg_height(rdgwindow->rdg)) / 2 - 12;

    uint64_t selected_node = rdg_get_node_by_coords(rdgwindow->rdg,
                                                    image_x, image_y);

    uint64_t selected_ins = rdg_get_ins_by_coords(rdgwindow->rdg,
                                                  rdgwindow->gui->rdis->graph,
                                                  image_x, image_y);

    if (selected_ins != -1)
        rdgwindow->selected_ins = selected_ins;

    if (selected_node == -1)
        return FALSE;

    rdgwindow->selected_node = selected_node;

    rdgwindow_color_node(rdgwindow);

    // right click for popup menu
    printf("selected_ins: %llx, event->button: %d\n",
           (unsigned long long) selected_ins,
           event->button);
    if ((selected_ins != -1) && (event->button == 3)) {
        rdgwindow_menu_popup(rdgwindow);
    }

    return FALSE;
}


gboolean rdgwindow_image_button_release_event (GtkWidget * widget,
                                               GdkEventButton * event,
                                               struct _rdgwindow * rdgwindow)
{
    rdgwindow->image_dragging = 0;

    return FALSE;
}


gboolean rdgwindow_image_key_press_event (GtkWidget * widget,
                                          GdkEventKey * event,
                                          struct _rdgwindow * rdgwindow)
{
    // key 'p'
    if ((event->keyval == ';') && (rdgwindow->selected_ins != -1)) {
        rdgwindow->editing = 1;
        return FALSE;
    }

    else if (event->keyval == GDK_KEY_Return) {
        rdgwindow->editing = 0;
        return FALSE;
    }

    if (rdgwindow->editing) {
        if ((event->keyval >= 0x20) && (event->keyval < 0x7f)) {
            struct _graph_node * node = graph_fetch_node(rdgwindow->gui->rdis->graph,
                                                         rdgwindow->selected_node);
            // find the selected instruction
            struct _list_it * it;
            struct _list * ins_list = node->data;
            for (it = list_iterator(ins_list); it != NULL; it = it->next) {
                struct _ins * ins = it->data;
                if (ins->address != rdgwindow->selected_ins)
                    continue;

                char tmpc[4];
                sprintf(tmpc, "%c", event->keyval);
                if (ins->comment == NULL) {
                    ins_s_comment(ins, tmpc);
                }
                else {
                    char * tmp = (char *) malloc(strlen(ins->comment) + 2);
                    strcpy(tmp, ins->comment);
                    strcat(tmp, tmpc);
                    ins_s_comment(ins, tmp);
                    free(tmp);
                }
                break;
            }
        }
        else if (event->keyval == GDK_KEY_BackSpace) {
            struct _graph_node * node = graph_fetch_node(rdgwindow->gui->rdis->graph,
                                                         rdgwindow->selected_node);
            struct _list_it * it;
            struct _list * ins_list = node->data;
            for (it = list_iterator(ins_list); it != NULL; it = it->next) {
                struct _ins * ins = it->data;
                if (ins->address != rdgwindow->selected_ins)
                    continue;

                if (ins->comment == NULL)
                    break;

                if (strlen(ins->comment) == 0)
                    break;

                char * tmp = strdup(ins->comment);
                tmp[strlen(tmp) - 1] = '\0';
                ins_s_comment(ins, tmp);
                free(tmp);
            }
        }
        // this callback will cause us to redraw the graph
        rdis_callback(rdgwindow->gui->rdis);
        return FALSE;
    }

    if (event->keyval == 0x70) {
        rdgwindow_color_node_predecessors(rdgwindow);
    }

    return FALSE;
}


void rdgwindow_size_allocate_event (GtkWidget * widget,
                                    GdkRectangle * allocation,
                                    struct _rdgwindow * rdgwindow)
{
    rdgwindow->scrolledWindow_width  = allocation->width;
    rdgwindow->scrolledWindow_height = allocation->height;
}


// sets rdgwindow->node_colors to a list that resets all nodes to the neutral color
void rdgwindow_reset_node_colors (struct _rdgwindow * rdgwindow)
{
    struct _list * node_colors = list_create();

    if (rdgwindow->node_colors != NULL) {
        struct _list_it * it;
        for (it = list_iterator(rdgwindow->node_colors); it != NULL; it = it->next) {
            struct _rdg_node_color * rdg_node_color = it->data;

            struct _rdg_node_color * new;
            new = rdg_node_color_create(rdg_node_color->index, RDG_NODE_BG_COLOR);
            list_append(node_colors, new);
            object_delete(new);
        }
        object_delete(rdgwindow->node_colors);
    }

    rdg_custom_nodes(rdgwindow->rdg,
                     rdgwindow->currently_displayed_graph,
                     rdgwindow->gui->rdis->labels,
                     node_colors,
                     rdgwindow->selected_ins);

    object_delete(node_colors);
    rdgwindow->node_colors = list_create();
}


void rdgwindow_color_node (struct _rdgwindow * rdgwindow)
{
    rdgwindow_reset_node_colors(rdgwindow);

    struct _rdg_node_color * rdg_node_color;
    rdg_node_color = rdg_node_color_create(rdgwindow->selected_node,
                                           RDGWINDOW_NODE_COLOR_SELECT);
    list_append(rdgwindow->node_colors, rdg_node_color);

    rdg_custom_nodes(rdgwindow->rdg,
                     rdgwindow->currently_displayed_graph,
                     rdgwindow->gui->rdis->labels,
                     rdgwindow->node_colors,
                     rdgwindow->selected_ins);
    rdgwindow_image_update(rdgwindow);
}


void rdgwindow_color_node_predecessors (struct _rdgwindow * rdgwindow)
{
    if (rdgwindow->selected_node == -1)
        return;

    struct _tree  * pre_tree = tree_create();
    struct _queue * queue    = queue_create();

    // add currently selected node to queue
    struct _index * index = index_create(rdgwindow->selected_node);
    queue_push(queue, index);
    object_delete(index);

    // add predecessors to the queue
    while (queue->size > 0) {
        index = queue_peek(queue);
        
        // if we've already added this node, skip it
        if (tree_fetch(pre_tree, index) != NULL) {
            queue_pop(queue);
            continue;
        }

        tree_insert(pre_tree, index);

        struct _graph_node * node = graph_fetch_node(rdgwindow->gui->rdis->graph,
                                                     index->index);
        struct _list * predecessors = graph_node_predecessors(node);
        struct _list_it * it;
        for (it = list_iterator(predecessors); it != NULL; it = it->next) {
            struct _graph_edge * edge = it->data;
            index = index_create(edge->head);
            queue_push(queue, index);
            index_delete(index);
        }

        queue_pop(queue);
    }

    object_delete(queue);

    rdgwindow_reset_node_colors(rdgwindow);

    // add predecessors to node_colors
    struct _tree_it * it;
    for (it = tree_iterator(pre_tree); it != NULL; it = tree_it_next(it)) {
        index = tree_it_data(it);

        struct _rdg_node_color * rdg_node_color;
        rdg_node_color = rdg_node_color_create(index->index,
                                               RDGWINDOW_NODE_COLOR_PRE);
        list_append(rdgwindow->node_colors, rdg_node_color);
        object_delete(rdg_node_color);
    }

    object_delete(pre_tree);

    rdg_color_nodes(rdgwindow->rdg,
                    rdgwindow->currently_displayed_graph,
                    rdgwindow->gui->rdis->labels,
                    rdgwindow->node_colors);
    rdgwindow_image_update(rdgwindow);
}


void rdgwindow_rdis_callback (struct _rdgwindow * rdgwindow)
{
    rdgwindow_graph_update(rdgwindow);
}



void rdgwindow_menu_popup (struct _rdgwindow * rdgwindow)
{
    gtk_menu_popup(GTK_MENU(rdgwindow->menu_popup),
                   NULL, NULL, NULL,
                   rdgwindow,
                   0,
                   gtk_get_current_event_time());
}



void rdgwindow_user_function (GtkMenuItem * menuItem,
                              struct _rdgwindow * rdgwindow)
{
    if (rdgwindow->selected_ins != -1)
    rdis_user_function(rdgwindow->gui->rdis, rdgwindow->selected_ins);

    printf("user function click on %llx\n",
           (unsigned long long) rdgwindow->selected_ins);
}



/*
gboolean inswindow_image_scroll_event (GtkWidget * widget,
                                       GdkEventScroll * event,
                                       struct _inswindow * inswindow)
{
    if (inswindow->image_drawing)
        return 1;

    int direction = -1;
    if (event->direction == GDK_SCROLL_SMOOTH) {
        double delta_x;
        double delta_y;
        gdk_event_get_scroll_deltas((GdkEvent *) event, &delta_x, &delta_y);
        if (delta_y < 0)
            direction = GDK_SCROLL_DOWN;
        else if (delta_y > 0)
            direction = GDK_SCROLL_UP;
    }
    else if (event->direction == GDK_SCROLL_UP)
        direction = GDK_SCROLL_UP;
    else if (event->direction == GDK_SCROLL_DOWN)
        direction = GDK_SCROLL_DOWN;


    if (direction == GDK_SCROLL_DOWN) {
        printf("scroll up\n");
        inswindow->image_zoom *= 1.1;
        if (inswindow->image_zoom > 10.0)
            inswindow->image_zoom = 10.0;
    }
    else if (direction == GDK_SCROLL_UP) {
        printf("scroll down\n");
        inswindow->image_zoom *= 0.9;
        if (inswindow->image_zoom < 0.25)
            inswindow->image_zoom = 0.25;
    }
    else
        return 1;

    //printf("image_zoom: %f\n", inswindow->image_zoom);
    printf("reduce and draw\n");
    rdg_graph_reduce_and_draw(inswindow->rdg_graph);

    inswindow_image_update(inswindow);

    return 1;
}
*/