#include <stdio.h>
#include <raylib.h>
#include <raymath.h>

#include "utils.h"

define_da_named(Vector2, spline)

void print_vector2(Vector2 v) {
    printf("Vector2 {\n"
            "\t%.3f,\n"
            "\t%.3f,\n"
           "}",
        v.x, v.y);
}

typedef enum {
    SELECT,
    PEN_TOOL,
    DRAW_SPLINE_MANUAL,
} EditorState;

typedef enum {
    IDLE,
    FIRST_NODE,
} PenToolState;

// global variables
static EditorState editor_state = SELECT;
static PenToolState pen_tool_state = IDLE;
static int dragged_control_point_idx = -1;
static int selected_control_point = -1;
static Vector2 mouse_pos;

char *editor_state_to_string(EditorState state) {
    switch(state) {
        case SELECT:
            return "Select";
        case PEN_TOOL:
            return "Pen Tool";
        case DRAW_SPLINE_MANUAL:
            return "Manual Spline";
        default:
            return "Invalid";
    }
}

Vector2 get_cubic_bezier_sample(Vector2 p1, Vector2 p2, Vector2 p3, Vector2 p4, float t) {
    Vector2 p12 = Vector2Lerp(p1, p2, t);
    Vector2 p23 = Vector2Lerp(p2, p3, t);
    Vector2 p34 = Vector2Lerp(p3, p4, t);

    Vector2 p123 = Vector2Lerp(p12, p23, t);
    Vector2 p234 = Vector2Lerp(p23, p34, t);

    Vector2 p1234 = Vector2Lerp(p123, p234, t);
    return p1234;
}

void draw_editor_state() {
    int width = GetScreenWidth();
    char *text = editor_state_to_string(editor_state);
    int font_size = 20;
    int text_width = MeasureText(text, font_size);
    DrawText(text, width - text_width - 10, 10, font_size, ORANGE);
}

void draw_spline_tangents(spline spline_da, Color color) {
    if(spline_da.length >= 2) {
        DrawLineV(spline_da.data[0], spline_da.data[1], color);
    }

    for(int i = 3; i < spline_da.length; i += 3) {
        DrawLineV(spline_da.data[i-1], spline_da.data[i], color);
        if(i + 1 < spline_da.length) {
            DrawLineV(spline_da.data[i], spline_da.data[i+1], color);
        }
    }
}

void draw_spline_segments(spline spline_da, Color color) {
    const int num_segments = 32;

    for(int i = 0; i < spline_da.length - 3; i+=3) {
        for(int j = 0; j < num_segments; j++) {
            float t1 = (float)j/num_segments;
            float t2 = (float)(j+1)/num_segments;
            Vector2 start = get_cubic_bezier_sample(spline_da.data[i], spline_da.data[i+1], spline_da.data[i+2], spline_da.data[i+3], t1);
            Vector2 end = get_cubic_bezier_sample(spline_da.data[i], spline_da.data[i+1], spline_da.data[i+2], spline_da.data[i+3], t2);
            DrawLineEx(start, end, 4, color);
        }
    }
}

void draw_spline_controls(spline spline_da, float control_point_radius) {
    for(int i = 0; i < spline_da.length; i++) {
        bool hovers = CheckCollisionPointCircle(mouse_pos, spline_da.data[i], control_point_radius);
        Color core_color = hovers ? BLUE: BLACK;
        if(i % 3 == 0) {
            DrawCircleV(spline_da.data[i], control_point_radius, WHITE);
            DrawCircleV(spline_da.data[i], 0.85*control_point_radius, BLACK);
            DrawCircleV(spline_da.data[i], 0.70*control_point_radius, WHITE);
            DrawCircleV(spline_da.data[i], 0.55*control_point_radius, core_color);
            continue;
        }
        Vector2 size = {2 * control_point_radius, 2 * control_point_radius};
        Vector2 pos = Vector2Subtract(spline_da.data[i], Vector2Scale(size, 0.5));
        DrawRectangleV(pos, size, WHITE);
        size = Vector2Scale(size, 0.9);
        pos = Vector2Subtract(spline_da.data[i], Vector2Scale(size, 0.5));
        DrawRectangleV(pos, size, BLACK);
        size = Vector2Scale(size, 0.9);
        pos = Vector2Subtract(spline_da.data[i], Vector2Scale(size, 0.5));
        DrawRectangleV(pos, size, WHITE);
        size = Vector2Scale(size, 0.9);
        pos = Vector2Subtract(spline_da.data[i], Vector2Scale(size, 0.5));
        DrawRectangleV(pos, size, core_color);

    }
}
void draw_spline_incomplete_at_mouse(spline *spline_da, float control_point_radius, bool draw_control_points, bool draw_tangents, Color spline_color, Color incomplete_color) {
    if(draw_tangents) {
        draw_spline_tangents(*spline_da, PURPLE);
    }
    draw_spline_segments(*spline_da, spline_color);

    spline_append(spline_da, mouse_pos);
    spline_append(spline_da, mouse_pos);
    spline new_spline = *spline_da;
    new_spline.data += new_spline.length - 4;
    new_spline.length = 4;
    draw_spline_segments(new_spline, incomplete_color);
    spline_da->length -= 2;

    if(draw_control_points) {
        draw_spline_controls(*spline_da, control_point_radius);
    }
}

void draw_spline(spline spline_da, float control_point_radius, bool draw_control_points, bool draw_tangents, Color spline_color) {

    if(draw_tangents) {
        draw_spline_tangents(spline_da, PURPLE);
    }

    draw_spline_segments(spline_da, spline_color);

    if(draw_control_points) {
        draw_spline_controls(spline_da, control_point_radius);
    }
}

int get_control_point_idx_colliding_with_point(spline spline_da, float control_point_radius, Vector2 point) {
    for(int i = 0; i < spline_da.length; i++) {
        if(CheckCollisionPointCircle(point, spline_da.data[i], control_point_radius)) {
            return i;
        }
    }
    return -1;
}
void update_bezier_handles_symmetrically(spline spline_da, int handle_idx, Vector2 handle_pos) {
    spline_da.data[handle_idx] = handle_pos;
    // NOTE: not checking if the dragged_control_idx is not a multiple of 3 is probably fine
    // assuming we will never pass it a multiple of 3; maybe throw in an assert here anyway?
    int symmetric_handle_idx = 0;
    if(handle_idx % 3 == 0) {
        fprintf(stderr, "Error from line %d: the index of a handle should not be divisible by 3\n", __LINE__);
        return;
    } else if(handle_idx % 3 == 1) {
        symmetric_handle_idx = handle_idx - 2;
    } else {
        symmetric_handle_idx = handle_idx + 2;
    }
    if(symmetric_handle_idx < 2 || symmetric_handle_idx >= spline_da.length) {
        printf("Log from line %d: the computed index %d of the symmetric_handle is out of bounds\n", __LINE__, symmetric_handle_idx);
        return;
    }

    Vector2 node_pos = spline_da.data[handle_idx - 1];
    Vector2 handle_pos_to_node_pos = Vector2Subtract(node_pos, spline_da.data[handle_idx]);
    Vector2 symmetric_handle_pos = Vector2Add(node_pos, handle_pos_to_node_pos);
    spline_da.data[symmetric_handle_idx] = symmetric_handle_pos;
}

void snap_control_point_to_nearest(spline spline_da, float control_point_radius, int control_point_idx) {
    if(control_point_idx < 0 || control_point_idx >= spline_da.length) {
        fprintf(stderr, "Error from line %d: control point index %d out of bounds [0, %d)\n", __LINE__, control_point_idx, spline_da.length);
        return;
    }
    int colliding_point_idx = get_control_point_idx_colliding_with_point(
        spline_da,
        control_point_radius,
        spline_da.data[control_point_idx]
    );
    if(colliding_point_idx >= 0) {
        spline_da.data[control_point_idx] = spline_da.data[colliding_point_idx];
    }
}

int main() {

    spline spline_da = spline_init(2);
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(800, 600, "Splines");
    SetExitKey(KEY_NULL);
    MaximizeWindow();
    SetTargetFPS(60);

    bool draw_control_points = true;
    bool draw_tangents = true;

    const float control_point_radius = 10;

    while(!WindowShouldClose()) {
        mouse_pos = GetMousePosition();

        switch(editor_state) {
            case SELECT: {
                break;
            }
            case PEN_TOOL: {
                if(IsKeyPressed(KEY_ESCAPE)) {
                    if(pen_tool_state != IDLE) {
                        spline_da.length = 0;
                        pen_tool_state = IDLE;
                    }
                }
                if(IsKeyPressed(KEY_ENTER)) {
                    pen_tool_state = IDLE;
                    spline_da.length--;
                }
                if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                    if(pen_tool_state == IDLE) {
                        // TODO: remove this temporary hack that draws a new spline every time
                        spline_da.length = 0;
                        spline_append(&spline_da, mouse_pos);
                        spline_append(&spline_da, mouse_pos);
                    } else if(pen_tool_state == FIRST_NODE) {
                        spline_append(&spline_da, mouse_pos);
                        spline_append(&spline_da, mouse_pos);
                        spline_append(&spline_da, mouse_pos);
                    }
                    dragged_control_point_idx = spline_da.length - 1;
                }
                if(IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
                    if(dragged_control_point_idx >= 0) {
                        snap_control_point_to_nearest(
                            spline_da,
                            control_point_radius,
                            dragged_control_point_idx
                        );
                    }
                    dragged_control_point_idx = -1;
                    if(pen_tool_state == IDLE) {
                        pen_tool_state = FIRST_NODE;    
                    }
                }
                if(IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
                    update_bezier_handles_symmetrically(spline_da, dragged_control_point_idx, mouse_pos);
                }
                break;
            }
            case DRAW_SPLINE_MANUAL: {
                if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                    dragged_control_point_idx = get_control_point_idx_colliding_with_point(
                        spline_da,
                        control_point_radius,
                        mouse_pos
                    );
                    if (dragged_control_point_idx < 0) {
                        spline_append(&spline_da, mouse_pos);
                    }
                }
                if(IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
                    if(dragged_control_point_idx >= 0) {
                        snap_control_point_to_nearest(
                            spline_da,
                            control_point_radius,
                            dragged_control_point_idx
                        );
                    }
                    dragged_control_point_idx = -1;
                }
                if(IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
                    if(dragged_control_point_idx >= 0) {
                        spline_da.data[dragged_control_point_idx] = mouse_pos;
                    }
                }
                break;
            }

        }
        if(IsKeyPressed(KEY_C)) {
            draw_control_points = !draw_control_points;
        }
        if(IsKeyPressed(KEY_T)) {
            draw_tangents = !draw_tangents;
        }

        if(IsKeyPressed(KEY_J)) {
            if(spline_da.length > 0) {
                spline_da.length--;
            }
        }
        if(IsKeyPressed(KEY_K)) {
            if(spline_da.length < spline_da.capacity) {
                spline_da.length++;
            }
        }

        if(IsKeyPressed(KEY_ONE)) {
            editor_state = SELECT;
        }
        if(IsKeyPressed(KEY_TWO)) {
            editor_state = PEN_TOOL;
        }
        if(IsKeyPressed(KEY_THREE)) {
            editor_state = DRAW_SPLINE_MANUAL;
        }

        if(dragged_control_point_idx >= 0) {
            spline_da.data[dragged_control_point_idx] = mouse_pos;
        }


        BeginDrawing();
        {
            ClearBackground(BLACK);
            for(int i = 0; i < spline_da.length; i++) {
                if(editor_state == PEN_TOOL && pen_tool_state != IDLE) {
                    draw_spline_incomplete_at_mouse(&spline_da, control_point_radius, draw_control_points, draw_tangents, WHITE, RED);
                } else {
                    draw_spline(spline_da, control_point_radius, draw_control_points, draw_tangents, WHITE);
                }
            }
            draw_editor_state(editor_state);
        }
        EndDrawing();
    }
    CloseWindow();
    return 0;
}