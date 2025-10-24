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
    DRAW_SPLINE_AUTO,
    DRAW_SPLINE_MANUAL,
} EditorState;

char *editor_state_to_string(EditorState state) {
    switch(state) {
        case SELECT:
            return "Select";
        case DRAW_SPLINE_AUTO:
            return "Auto Spline";
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

void draw_editor_state(EditorState state) {
    int width = GetScreenWidth();
    char *text = editor_state_to_string(state);
    int font_size = 20;
    int text_width = MeasureText(text, font_size);
    DrawText(text, width - text_width - 10, 10, font_size, ORANGE);
}

void draw_spline(spline spline_da, float control_point_radius, bool draw_control_points, bool draw_connecting_lines, Color spline_color) {

    if(draw_connecting_lines) {
        for(int i = 0; i < spline_da.length - 1; i++) {
            DrawLineV(spline_da.data[i], spline_da.data[i+1], GREEN);
        }
    }

    const int num_segments = 40;

    for(int i = 0; i < spline_da.length - 3; i+=3) {
        for(int j = 0; j < num_segments; j++) {
            float t1 = (float)j/num_segments;
            float t2 = (float)(j+1)/num_segments;
            Vector2 start = get_cubic_bezier_sample(spline_da.data[i], spline_da.data[i+1], spline_da.data[i+2], spline_da.data[i+3], t1);
            Vector2 end = get_cubic_bezier_sample(spline_da.data[i], spline_da.data[i+1], spline_da.data[i+2], spline_da.data[i+3], t2);
            DrawLineEx(start, end, 4, spline_color);
        }
    }

    if(draw_control_points) {
        for(int i = 0; i < spline_da.length; i++) {
            DrawCircleV(spline_da.data[i], 5, RED);
        }
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


int main() {

    spline spline_da = spline_init(2);
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(800, 600, "Splines");
    MaximizeWindow();
    SetTargetFPS(60);

    EditorState editor_state = SELECT;

    bool draw_control_points = true;
    bool draw_connecting_lines = true;

    const float control_point_radius = 10;

    int dragged_control_point_idx = -1;
    
    while(!WindowShouldClose()) {
        Vector2 mouse_pos = GetMousePosition();
        if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            dragged_control_point_idx = get_control_point_idx_colliding_with_point(spline_da, control_point_radius, mouse_pos);
            if (dragged_control_point_idx < 0) {
                spline_append(&spline_da, mouse_pos);
            }
        }
        if(IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
            dragged_control_point_idx = -1;
        }
        if(IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
            if(dragged_control_point_idx >= 0) {
                spline_da.data[dragged_control_point_idx] = mouse_pos;
            }
        }
        if(IsKeyPressed(KEY_C)) {
            draw_control_points = !draw_control_points;
        }
        if(IsKeyPressed(KEY_T)) {
            draw_connecting_lines = !draw_connecting_lines;
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
            editor_state = DRAW_SPLINE_AUTO;
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
                draw_spline(spline_da, control_point_radius, draw_control_points, draw_connecting_lines, WHITE);
            }
            draw_editor_state(editor_state);
        }
        EndDrawing();
    }
    CloseWindow();
    return 0;
}