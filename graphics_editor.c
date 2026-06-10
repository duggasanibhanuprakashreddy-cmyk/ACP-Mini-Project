#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#define WIDTH 80
#define HEIGHT 20
#define MAX_OBJECTS 64

typedef enum {
    SHAPE_LINE,
    SHAPE_RECTANGLE,
    SHAPE_CIRCLE,
    SHAPE_TRIANGLE
} ShapeType;

typedef struct {
    int x1, y1;
    int x2, y2;
} LineParams;

typedef struct {
    int x, y; // Top-left corner
    int width, height;
} RectParams;

typedef struct {
    int cx, cy; // Center
    int r;      // Radius
} CircleParams;

typedef struct {
    int x1, y1;
    int x2, y2;
    int x3, y3;
} TriangleParams;

typedef struct {
    ShapeType type;
    union {
        LineParams line;
        RectParams rect;
        CircleParams circle;
        TriangleParams triangle;
    } params;
} GraphicObject;

// Function declarations
void init_canvas(char canvas[HEIGHT][WIDTH]);
void display_canvas(const char canvas[HEIGHT][WIDTH]);
void plot_point(char canvas[HEIGHT][WIDTH], int x, int y);
void draw_line(char canvas[HEIGHT][WIDTH], int x1, int y1, int x2, int y2);
void draw_circle(char canvas[HEIGHT][WIDTH], int cx, int cy, int r);
void draw_rectangle(char canvas[HEIGHT][WIDTH], int x, int y, int w, int h);
void draw_triangle(char canvas[HEIGHT][WIDTH], int x1, int y1, int x2, int y2, int x3, int y3);

void render_all_objects(char canvas[HEIGHT][WIDTH], GraphicObject objects[], int object_count);
void add_object(GraphicObject objects[], int *object_count);
void delete_object(GraphicObject objects[], int *object_count);
void modify_object(GraphicObject objects[], int object_count);
void list_objects(GraphicObject objects[], int object_count);

int get_int(const char *prompt);
void print_menu();

int main() {
    char canvas[HEIGHT][WIDTH];
    GraphicObject objects[MAX_OBJECTS];
    int object_count = 0;

    init_canvas(canvas);

    while (1) {
        print_menu();
        int option = get_int("Select option (1-6): ");
        printf("\n");
        switch (option) {
            case 1:
                render_all_objects(canvas, objects, object_count);
                display_canvas(canvas);
                break;
            case 2:
                add_object(objects, &object_count);
                render_all_objects(canvas, objects, object_count);
                break;
            case 3:
                delete_object(objects, &object_count);
                render_all_objects(canvas, objects, object_count);
                break;
            case 4:
                modify_object(objects, object_count);
                render_all_objects(canvas, objects, object_count);
                break;
            case 5:
                object_count = 0;
                render_all_objects(canvas, objects, object_count);
                printf("All objects cleared.\n");
                break;
            case 6:
                printf("Exiting 2D Graphics Editor. Goodbye!\n");
                return 0;
            default:
                printf("Invalid option. Please choose between 1 and 6.\n");
                break;
        }
    }
    return 0;
}

// Initialize canvas with underscores
void init_canvas(char canvas[HEIGHT][WIDTH]) {
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            canvas[y][x] = '_';
        }
    }
}

// Display the canvas with row and column guides
void display_canvas(const char canvas[HEIGHT][WIDTH]) {
    // Print column header tens digit
    printf("   ");
    for (int x = 0; x < WIDTH; x++) {
        if (x % 10 == 0) {
            printf("%d", (x / 10) % 10);
        } else {
            printf(" ");
        }
    }
    printf("\n");

    // Print column header units digit
    printf("   ");
    for (int x = 0; x < WIDTH; x++) {
        printf("%d", x % 10);
    }
    printf("\n");

    // Print top border line
    printf("  +");
    for (int x = 0; x < WIDTH; x++) printf("-");
    printf("+\n");

    // Print rows with row indices
    for (int y = 0; y < HEIGHT; y++) {
        printf("%2d|", y);
        for (int x = 0; x < WIDTH; x++) {
            printf("%c", canvas[y][x]);
        }
        printf("|%2d\n", y);
    }

    // Print bottom border line
    printf("  +");
    for (int x = 0; x < WIDTH; x++) printf("-");
    printf("+\n");

    // Print column footer units digit
    printf("   ");
    for (int x = 0; x < WIDTH; x++) {
        printf("%d", x % 10);
    }
    printf("\n");
}

// Plot a point if it's within bounds
void plot_point(char canvas[HEIGHT][WIDTH], int x, int y) {
    if (x >= 0 && x < WIDTH && y >= 0 && y < HEIGHT) {
        canvas[y][x] = '*';
    }
}

// Bresenham's Line Drawing Algorithm
void draw_line(char canvas[HEIGHT][WIDTH], int x1, int y1, int x2, int y2) {
    int dx = abs(x2 - x1);
    int dy = -abs(y2 - y1);
    int sx = x1 < x2 ? 1 : -1;
    int sy = y1 < y2 ? 1 : -1;
    int err = dx + dy;
    int e2;

    while (1) {
        plot_point(canvas, x1, y1);
        if (x1 == x2 && y1 == y2) break;
        e2 = 2 * err;
        if (e2 >= dy) {
            err += dy;
            x1 += sx;
        }
        if (e2 <= dx) {
            err += dx;
            y1 += sy;
        }
    }
}

// Helper function to plot 8 octant points for circle drawing
static void plot_circle_points(char canvas[HEIGHT][WIDTH], int cx, int cy, int x, int y) {
    plot_point(canvas, cx + x, cy + y);
    plot_point(canvas, cx - x, cy + y);
    plot_point(canvas, cx + x, cy - y);
    plot_point(canvas, cx - x, cy - y);
    plot_point(canvas, cx + y, cy + x);
    plot_point(canvas, cx - y, cy + x);
    plot_point(canvas, cx + y, cy - x);
    plot_point(canvas, cx - y, cy - x);
}

// Bresenham's (Midpoint) Circle Algorithm
void draw_circle(char canvas[HEIGHT][WIDTH], int cx, int cy, int r) {
    if (r < 0) return;
    if (r == 0) {
        plot_point(canvas, cx, cy);
        return;
    }
    int x = 0;
    int y = r;
    int d = 3 - 2 * r;

    plot_circle_points(canvas, cx, cy, x, y);
    while (y >= x) {
        x++;
        if (d > 0) {
            y--;
            d = d + 4 * (x - y) + 10;
        } else {
            d = d + 4 * x + 6;
        }
        plot_circle_points(canvas, cx, cy, x, y);
    }
}

// Rectangle drawing (outlines only)
void draw_rectangle(char canvas[HEIGHT][WIDTH], int x, int y, int w, int h) {
    if (w <= 0 || h <= 0) return;
    
    // Draw top and bottom borders
    for (int i = x; i < x + w; i++) {
        plot_point(canvas, i, y);
        plot_point(canvas, i, y + h - 1);
    }
    // Draw left and right borders
    for (int i = y; i < y + h; i++) {
        plot_point(canvas, x, i);
        plot_point(canvas, x + w - 1, i);
    }
}

// Triangle drawing (outlines only, connects three points with lines)
void draw_triangle(char canvas[HEIGHT][WIDTH], int x1, int y1, int x2, int y2, int x3, int y3) {
    draw_line(canvas, x1, y1, x2, y2);
    draw_line(canvas, x2, y2, x3, y3);
    draw_line(canvas, x3, y3, x1, y1);
}

// Render all objects onto the canvas
void render_all_objects(char canvas[HEIGHT][WIDTH], GraphicObject objects[], int object_count) {
    init_canvas(canvas);
    for (int i = 0; i < object_count; i++) {
        switch (objects[i].type) {
            case SHAPE_LINE:
                draw_line(canvas, objects[i].params.line.x1, objects[i].params.line.y1,
                          objects[i].params.line.x2, objects[i].params.line.y2);
                break;
            case SHAPE_RECTANGLE:
                draw_rectangle(canvas, objects[i].params.rect.x, objects[i].params.rect.y,
                               objects[i].params.rect.width, objects[i].params.rect.height);
                break;
            case SHAPE_CIRCLE:
                draw_circle(canvas, objects[i].params.circle.cx, objects[i].params.circle.cy,
                            objects[i].params.circle.r);
                break;
            case SHAPE_TRIANGLE:
                draw_triangle(canvas, objects[i].params.triangle.x1, objects[i].params.triangle.y1,
                              objects[i].params.triangle.x2, objects[i].params.triangle.y2,
                              objects[i].params.triangle.x3, objects[i].params.triangle.y3);
                break;
        }
    }
}

// Add an object to the list
void add_object(GraphicObject objects[], int *object_count) {
    if (*object_count >= MAX_OBJECTS) {
        printf("Error: Maximum object limit (%d) reached. Please delete an object first.\n", MAX_OBJECTS);
        return;
    }

    printf("Choose shape to add:\n");
    printf("  1. Line\n");
    printf("  2. Rectangle\n");
    printf("  3. Circle\n");
    printf("  4. Triangle\n");
    int choice = get_int("Enter choice (1-4): ");
    if (choice < 1 || choice > 4) {
        printf("Invalid choice. Returning to main menu.\n");
        return;
    }

    GraphicObject obj;
    if (choice == 1) {
        obj.type = SHAPE_LINE;
        obj.params.line.x1 = get_int("Enter X1: ");
        obj.params.line.y1 = get_int("Enter Y1: ");
        obj.params.line.x2 = get_int("Enter X2: ");
        obj.params.line.y2 = get_int("Enter Y2: ");
    } else if (choice == 2) {
        obj.type = SHAPE_RECTANGLE;
        obj.params.rect.x = get_int("Enter top-left X: ");
        obj.params.rect.y = get_int("Enter top-left Y: ");
        obj.params.rect.width = get_int("Enter Width: ");
        obj.params.rect.height = get_int("Enter Height: ");
        if (obj.params.rect.width <= 0 || obj.params.rect.height <= 0) {
            printf("Width and Height must be positive. Aborting.\n");
            return;
        }
    } else if (choice == 3) {
        obj.type = SHAPE_CIRCLE;
        obj.params.circle.cx = get_int("Enter Center X: ");
        obj.params.circle.cy = get_int("Enter Center Y: ");
        obj.params.circle.r = get_int("Enter Radius: ");
        if (obj.params.circle.r < 0) {
            printf("Radius cannot be negative. Aborting.\n");
            return;
        }
    } else if (choice == 4) {
        obj.type = SHAPE_TRIANGLE;
        obj.params.triangle.x1 = get_int("Enter X1: ");
        obj.params.triangle.y1 = get_int("Enter Y1: ");
        obj.params.triangle.x2 = get_int("Enter X2: ");
        obj.params.triangle.y2 = get_int("Enter Y2: ");
        obj.params.triangle.x3 = get_int("Enter X3: ");
        obj.params.triangle.y3 = get_int("Enter Y3: ");
    }

    // Quick checks for standard bounds coordinates warning (doesn't block adding)
    int out_of_bounds = 0;
    if (choice == 1) {
        if (obj.params.line.x1 < 0 || obj.params.line.x1 >= WIDTH || obj.params.line.y1 < 0 || obj.params.line.y1 >= HEIGHT ||
            obj.params.line.x2 < 0 || obj.params.line.x2 >= WIDTH || obj.params.line.y2 < 0 || obj.params.line.y2 >= HEIGHT) {
            out_of_bounds = 1;
        }
    } else if (choice == 2) {
        if (obj.params.rect.x < 0 || obj.params.rect.x >= WIDTH || obj.params.rect.y < 0 || obj.params.rect.y >= HEIGHT) {
            out_of_bounds = 1;
        }
    } else if (choice == 3) {
        if (obj.params.circle.cx < 0 || obj.params.circle.cx >= WIDTH || obj.params.circle.cy < 0 || obj.params.circle.cy >= HEIGHT) {
            out_of_bounds = 1;
        }
    } else if (choice == 4) {
        if (obj.params.triangle.x1 < 0 || obj.params.triangle.x1 >= WIDTH || obj.params.triangle.y1 < 0 || obj.params.triangle.y1 >= HEIGHT ||
            obj.params.triangle.x2 < 0 || obj.params.triangle.x2 >= WIDTH || obj.params.triangle.y2 < 0 || obj.params.triangle.y2 >= HEIGHT ||
            obj.params.triangle.x3 < 0 || obj.params.triangle.x3 >= WIDTH || obj.params.triangle.y3 < 0 || obj.params.triangle.y3 >= HEIGHT) {
            out_of_bounds = 1;
        }
    }

    if (out_of_bounds) {
        printf("Warning: Some coordinate inputs lie outside the canvas bounds (%d x %d).\n", WIDTH, HEIGHT);
        printf("The shape will be cropped during rendering.\n");
    }

    objects[*object_count] = obj;
    (*object_count)++;
    printf("Object added successfully!\n");
}

// Delete an object from the list
void delete_object(GraphicObject objects[], int *object_count) {
    if (*object_count == 0) {
        printf("No objects to delete.\n");
        return;
    }

    list_objects(objects, *object_count);
    int index = get_int("Enter index of object to delete: ");
    if (index < 0 || index >= *object_count) {
        printf("Invalid index.\n");
        return;
    }

    // Shift elements left
    for (int i = index; i < *object_count - 1; i++) {
        objects[i] = objects[i + 1];
    }
    (*object_count)--;
    printf("Object deleted successfully.\n");
}

// Modify an existing object in the list
void modify_object(GraphicObject objects[], int object_count) {
    if (object_count == 0) {
        printf("No objects to modify.\n");
        return;
    }

    list_objects(objects, object_count);
    int index = get_int("Enter index of object to modify: ");
    if (index < 0 || index >= object_count) {
        printf("Invalid index.\n");
        return;
    }

    GraphicObject *obj = &objects[index];
    printf("Modifying object [%d].\n", index);

    switch (obj->type) {
        case SHAPE_LINE:
            printf("Current: Line: (%d, %d) to (%d, %d)\n", 
                   obj->params.line.x1, obj->params.line.y1,
                   obj->params.line.x2, obj->params.line.y2);
            obj->params.line.x1 = get_int("Enter new X1: ");
            obj->params.line.y1 = get_int("Enter new Y1: ");
            obj->params.line.x2 = get_int("Enter new X2: ");
            obj->params.line.y2 = get_int("Enter new Y2: ");
            break;
        case SHAPE_RECTANGLE:
            printf("Current: Rectangle: Top-Left (%d, %d), Width %d, Height %d\n", 
                   obj->params.rect.x, obj->params.rect.y,
                   obj->params.rect.width, obj->params.rect.height);
            obj->params.rect.x = get_int("Enter new top-left X: ");
            obj->params.rect.y = get_int("Enter new top-left Y: ");
            int new_w = get_int("Enter new Width: ");
            int new_h = get_int("Enter new Height: ");
            if (new_w <= 0 || new_h <= 0) {
                printf("Width and Height must be positive. Modification aborted.\n");
                return;
            }
            obj->params.rect.width = new_w;
            obj->params.rect.height = new_h;
            break;
        case SHAPE_CIRCLE:
            printf("Current: Circle: Center (%d, %d), Radius %d\n", 
                   obj->params.circle.cx, obj->params.circle.cy,
                   obj->params.circle.r);
            obj->params.circle.cx = get_int("Enter new Center X: ");
            obj->params.circle.cy = get_int("Enter new Center Y: ");
            int new_r = get_int("Enter new Radius: ");
            if (new_r < 0) {
                printf("Radius cannot be negative. Modification aborted.\n");
                return;
            }
            obj->params.circle.r = new_r;
            break;
        case SHAPE_TRIANGLE:
            printf("Current: Triangle: (%d, %d), (%d, %d), (%d, %d)\n", 
                   obj->params.triangle.x1, obj->params.triangle.y1,
                   obj->params.triangle.x2, obj->params.triangle.y2,
                   obj->params.triangle.x3, obj->params.triangle.y3);
            obj->params.triangle.x1 = get_int("Enter new X1: ");
            obj->params.triangle.y1 = get_int("Enter new Y1: ");
            obj->params.triangle.x2 = get_int("Enter new X2: ");
            obj->params.triangle.y2 = get_int("Enter new Y2: ");
            obj->params.triangle.x3 = get_int("Enter new X3: ");
            obj->params.triangle.y3 = get_int("Enter new Y3: ");
            break;
    }
    printf("Object modified successfully.\n");
}

// Print the list of active objects
void list_objects(GraphicObject objects[], int object_count) {
    if (object_count == 0) {
        printf("No objects currently in the picture.\n");
        return;
    }
    printf("Active Objects:\n");
    for (int i = 0; i < object_count; i++) {
        printf("  [%d] ", i);
        switch (objects[i].type) {
            case SHAPE_LINE:
                printf("Line: (%d, %d) to (%d, %d)\n", 
                       objects[i].params.line.x1, objects[i].params.line.y1,
                       objects[i].params.line.x2, objects[i].params.line.y2);
                break;
            case SHAPE_RECTANGLE:
                printf("Rectangle: Top-Left (%d, %d), Width %d, Height %d\n", 
                       objects[i].params.rect.x, objects[i].params.rect.y,
                       objects[i].params.rect.width, objects[i].params.rect.height);
                break;
            case SHAPE_CIRCLE:
                printf("Circle: Center (%d, %d), Radius %d\n", 
                       objects[i].params.circle.cx, objects[i].params.circle.cy,
                       objects[i].params.circle.r);
                break;
            case SHAPE_TRIANGLE:
                printf("Triangle: (%d, %d), (%d, %d), (%d, %d)\n", 
                       objects[i].params.triangle.x1, objects[i].params.triangle.y1,
                       objects[i].params.triangle.x2, objects[i].params.triangle.y2,
                       objects[i].params.triangle.x3, objects[i].params.triangle.y3);
                break;
        }
    }
}

// Robust input reading for integers
int get_int(const char *prompt) {
    int val;
    char buffer[256];
    while (1) {
        printf("%s", prompt);
        if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
            // EOF reached
            return -1;
        }
        // Trim newline
        buffer[strcspn(buffer, "\n")] = 0;
        // Parse int
        char *endptr;
        long parsed = strtol(buffer, &endptr, 10);
        // Skip leading whitespace in endptr
        while (*endptr && isspace((unsigned char)*endptr)) {
            endptr++;
        }
        if (endptr == buffer || *endptr != '\0') {
            printf("Invalid input. Please enter a valid integer.\n");
            continue;
        }
        val = (int)parsed;
        return val;
    }
}

// Print main menu options
void print_menu() {
    printf("\n=== 2D GRAPHICS EDITOR ===\n");
    printf("1. Display Canvas\n");
    printf("2. Add an Object\n");
    printf("3. Delete an Object\n");
    printf("4. Modify an Object\n");
    printf("5. Clear All Objects\n");
    printf("6. Exit\n");
}
