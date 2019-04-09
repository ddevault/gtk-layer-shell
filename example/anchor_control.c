#include "example.h"

typedef enum {
    ANCHOR_EDGE_LEFT = 0,
    ANCHOR_EDGE_RIGHT,
    ANCHOR_EDGE_TOP,
    ANCHOR_EDGE_BOTTOM,
    ANCHOR_EDGE_ENUM_COUNT,
} AnchorEdge;

void (*const anchor_edge_setters[]) (GtkWindow *window, gboolean anchor) = {
    gtk_layer_set_anchor_left,
    gtk_layer_set_anchor_right,
    gtk_layer_set_anchor_top,
    gtk_layer_set_anchor_bottom,
};

typedef struct {
    gboolean edges[ANCHOR_EDGE_ENUM_COUNT];
} AnchorEdges;

typedef struct {
    AnchorEdge edge;
    AnchorEdges *window_edges;
    GtkWindow *layer_window;
} AnchorButtonData;

static void
anchor_button_update (GtkButton *button, AnchorButtonData *data)
{
    gtk_button_set_relief (button,
                           data->window_edges->edges[data->edge] ?
                               GTK_RELIEF_NORMAL :
                               GTK_RELIEF_NONE);
}

static void
on_anchor_toggled (GtkButton *button, AnchorButtonData *data)
{
    gboolean is_anchored = !data->window_edges->edges[data->edge];
    data->window_edges->edges[data->edge] = is_anchored;
    anchor_button_update (button, data);
    g_return_if_fail (data->edge < sizeof (anchor_edge_setters) / sizeof (anchor_edge_setters[0]));
    anchor_edge_setters[data->edge] (data->layer_window, is_anchored);
}

static void
anchor_edge_setup_button (GtkButton *button, GtkWindow *layer_window, AnchorEdges *window_edges, AnchorEdge edge)
{
    AnchorButtonData *data = g_new0 (AnchorButtonData, 1);
    *data = (AnchorButtonData) {
        .edge = edge,
        .window_edges = window_edges,
        .layer_window = layer_window,
    };
    g_signal_connect_data (button,
                           "clicked",
                           G_CALLBACK (on_anchor_toggled),
                           data,
                           (GClosureNotify)g_free,
                           (GConnectFlags)0);
    anchor_button_update (button, data);
}

GtkWidget *
anchor_control_new (GtkWindow *layer_window,
                    gboolean default_left,
                    gboolean default_right,
                    gboolean default_top,
                    gboolean default_bottom)
{
    AnchorEdges *anchor_edges = g_new0 (AnchorEdges, 1);
    anchor_edges->edges[ANCHOR_EDGE_LEFT] = default_left;
    anchor_edges->edges[ANCHOR_EDGE_RIGHT] = default_right;
    anchor_edges->edges[ANCHOR_EDGE_TOP] = default_top;
    anchor_edges->edges[ANCHOR_EDGE_BOTTOM] = default_bottom;
    // This is never accessed, but is set for memeory management
    g_object_set_data_full (G_OBJECT (layer_window), "anchor_edges", anchor_edges, g_free);

    GtkWidget *outside_hbox = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
    {
        GtkWidget *hbox = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 5);
        gtk_box_pack_start (GTK_BOX (outside_hbox), hbox, TRUE, FALSE, 0);
        {
            GtkWidget *vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
            gtk_container_add (GTK_CONTAINER (hbox), vbox);
            {
                GtkWidget *button = gtk_button_new_from_icon_name ("go-first", GTK_ICON_SIZE_BUTTON);
                gtk_box_pack_start (GTK_BOX (vbox), button, TRUE, FALSE, 0);
                anchor_edge_setup_button (GTK_BUTTON (button), layer_window, anchor_edges, ANCHOR_EDGE_LEFT);
            }
        }{
            GtkWidget *center_vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 16);
            gtk_container_add (GTK_CONTAINER (hbox), center_vbox);
            {
                GtkWidget *button = gtk_button_new_from_icon_name ("go-top", GTK_ICON_SIZE_BUTTON);
                gtk_box_pack_start (GTK_BOX (center_vbox), button, FALSE, FALSE, 0);
                anchor_edge_setup_button (GTK_BUTTON (button), layer_window, anchor_edges, ANCHOR_EDGE_TOP);
            }{
                GtkWidget *button = gtk_button_new_from_icon_name ("go-bottom", GTK_ICON_SIZE_BUTTON);
                gtk_box_pack_end (GTK_BOX (center_vbox), button, FALSE, FALSE, 0);
                anchor_edge_setup_button (GTK_BUTTON (button), layer_window, anchor_edges, ANCHOR_EDGE_BOTTOM);
            }
        }{
            GtkWidget *vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
            gtk_container_add (GTK_CONTAINER (hbox), vbox);
            {
                GtkWidget *button = gtk_button_new_from_icon_name ("go-last", GTK_ICON_SIZE_BUTTON);
                gtk_box_pack_start (GTK_BOX (vbox), button, TRUE, FALSE, 0);
                anchor_edge_setup_button (GTK_BUTTON (button), layer_window, anchor_edges, ANCHOR_EDGE_RIGHT);
            }
        }
    }

    return outside_hbox;
}