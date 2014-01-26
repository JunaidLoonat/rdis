#ifndef rdis_lua_HEADER
#define rdis_lua_HEADER

#include <luajit-2.0/lua.h>
#include <luajit-2.0/lauxlib.h>
#include <luajit-2.0/lualib.h>

#include "instruction.h"
#include "rdg.h"
#include "rdis.h"

struct _rdis_lua {
    struct _rdis * rdis;
    lua_State * L;
};

struct _rdis_lua * rdis_lua_create  (struct _rdis * rdis);
void               rdis_lua_delete  (struct _rdis_lua * rdis_lua);

int                rdis_lua_execute (struct _rdis_lua * rdis_lua,
                                     const char * string);

int                rdis_lua_dofile  (struct _rdis_lua * rdis_lua,
                                     const char * filename);

int      rl_uint64_push  (lua_State * L, uint64_t value);
uint64_t rl_check_uint64 (lua_State * L, int position);

int rl_uint64          (lua_State * L);
int rl_uint64_add      (lua_State * L);
int rl_uint64_sub      (lua_State * L);
int rl_uint64_mul      (lua_State * L);
int rl_uint64_div      (lua_State * L);
int rl_uint64_mod      (lua_State * L);
int rl_uint64_eq       (lua_State * L);
int rl_uint64_lt       (lua_State * L);
int rl_uint64_le       (lua_State * L);
int rl_uint64_tostring (lua_State * L);
int rl_uint64_number   (lua_State * L);

int           rl_ins_push  (lua_State * L, struct _ins * ins);
struct _ins * rl_check_ins (lua_State * L, int position);

int rl_ins_gc          (lua_State * L);
int rl_ins_address     (lua_State * L);
int rl_ins_target      (lua_State * L);
int rl_ins_bytes       (lua_State * L);
int rl_ins_description (lua_State * L);
int rl_ins_comment     (lua_State * L);

int             rl_graph_push  (lua_State * L, struct _graph * graph);
struct _graph * rl_check_graph (lua_State * L, int position);

int rl_graph_gc      (lua_State * L);
int rl_graph_reduce  (lua_State * L);
int rl_graph_family  (lua_State * L);
int rl_graph_display (lua_State * L);

int                  rl_graph_edge_push  (lua_State * L,
                                          struct _graph_edge * edge);
struct _graph_edge * rl_check_graph_edge (lua_State * L, int position);

int rl_graph_edge_gc   (lua_State * L);
int rl_graph_edge_head (lua_State * L);
int rl_graph_edge_tail (lua_State * L);

int                  rl_graph_node_push  (lua_State * L,
                                          struct _graph_node * node);
struct _graph_node * rl_check_graph_node (lua_State * L, int position);

int rl_graph_node_gc           (lua_State * L);
int rl_graph_node_index        (lua_State * L);
int rl_graph_node_edges        (lua_State * L);
int rl_graph_node_instructions (lua_State * L);

int           rl_rdg_push  (lua_State * L, struct _rdg * rdg);
struct _rdg * rl_check_rdg (lua_State * L, int position);

int rl_rdg_gc             (lua_State * L);
int rl_rdg_save_png       (lua_State * L);
int rl_rdg_node_by_coords (lua_State * L);
int rl_rdg_ins_by_coords  (lua_State * L);
int rl_rdg_highlight_ins  (lua_State * L);

struct _rdis_lua * rl_get_rdis_lua (lua_State * L);

int rl_rdis_console            (lua_State * L);
int rl_rdis_functions          (lua_State * L);
int rl_rdis_peek               (lua_State * L);
int rl_rdis_poke               (lua_State * L);
int rl_rdis_node               (lua_State * L);
int rl_rdis_nodes              (lua_State * L);
int rl_rdis_load               (lua_State * L);
int rl_rdis_loader             (lua_State * L);
int rl_rdis_set_function_label (lua_State * L);
int rl_rdis_set_ins_comment    (lua_State * L);
int rl_rdis_sha256             (lua_State * L);
int rl_rdis_user_function      (lua_State * L);
int rl_rdis_dump_json          (lua_State * L);
int rl_rdis_rdg                (lua_State * L);
int rl_rdis_redis_x86          (lua_State * L);
int rl_rdis_setting            (lua_State * L);
int rl_rdis_entry              (lua_State * L);

#endif