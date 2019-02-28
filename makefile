textEditor: textEditor.c
	gcc textEditor.c `pkg-config --cflags --libs gtk+-3.0` -o a