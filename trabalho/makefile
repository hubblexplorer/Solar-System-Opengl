
IMGUI_DIR = imgui
SOURCES = main.cpp textFile.cpp stb_image.h
SOURCES += $(IMGUI_DIR)/imgui.cpp $(IMGUI_DIR)/imgui_demo.cpp $(IMGUI_DIR)/imgui_draw.cpp $(IMGUI_DIR)/imgui_tables.cpp $(IMGUI_DIR)/imgui_widgets.cpp
SOURCES += $(IMGUI_DIR)/imgui_impl_glfw.cpp $(IMGUI_DIR)/imgui_impl_opengl3.cpp
all:
	g++ $(SOURCES) -lGL -lglut -lGLEW -lglfw -lGL -lm -lX11 -lpthread -lXi -lXrandr -ldl -lSDL2 stb_image.h
