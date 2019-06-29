#ifndef EDITOR_INPUT_H
#define EDITOR_INPUT_H

#include <GLFW/glfw3.h>
#include <stdbool.h>

typedef struct Key{
	bool pressed;
	bool Released;
}Key;

typedef struct Input {
	struct Key D;
	struct Key A;
	struct Key E;
	struct Key Q;
	struct Key X;
	struct Key Z;
	struct Key W;
	struct Key S;
	struct Key C;
	struct Key V;
	struct Key G;
	struct Key TAB;
	struct Key KEY_1;
	struct Key KEY_2;
	struct Key KEY_3;
	struct Key SPACE;
	struct Key ESC;
	struct Key SHIFT;
	struct Key R;
	struct Key ENTER;
	struct Key J;
	struct Key K;
	struct Key T;
	struct Key L;
	struct Key I;
	struct Key O;
	struct Key KEY_0;
	struct Key KEY_9;
	struct Key P;
}Input;


struct Input input;

void update_input();
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

void init_input();

bool mouse_navigate_control;

#endif // !EDITOR_INPUT_H
