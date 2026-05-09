#define SDL_MAIN_USE_CALLBACKS 1

#include "libs/tinyexpr.h"

#include <stdio.h>
#include <printf.h>
#include <math.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include <iso646.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_error.h>
#include <SDL3/SDL_log.h>
#include <SDL3/SDL_pixels.h>
#include <SDL3/SDL_rect.h> 
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_stdinc.h>
#include <SDL3/SDL_video.h>


SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;


typedef struct{
	char* formula;
	SDL_FPoint* points;
	int colorR;
	int colorG;
	int colorB;

}Function;

typedef struct{
	void* data;
	int capacity;
	int size;
	size_t elementSize;
}DynamicArray;

typedef struct{
	int scale;
	int windowWidth;
	int windowHeight;
	int lastWindowWidth;
	int lastWindowHeight;
	char* formula;
	DynamicArray formulas;

}AppData;

void initArray(DynamicArray *arr, size_t elementSize, int initialCapacity){
	arr->elementSize = elementSize;
	arr->capacity = initialCapacity;
	arr->size = 0;
	arr->data = SDL_calloc(1, initialCapacity * elementSize);
}

void appendArray(DynamicArray *arr, void* element){
	if (arr->size >= arr->capacity){
		arr->capacity *= 2;
		arr->data = SDL_realloc(arr->data, arr->elementSize * arr->capacity);
	}

	void* target = (char*)arr->data + (arr->size * arr->elementSize);
	memcpy(target, element, arr->elementSize);

	arr->size++;
}
void* getArray(DynamicArray *arr, int index){
	return (char*)arr->data + (index * arr->elementSize);
}

SDL_FPoint* GetPoints(char* formula, int windowWidth, int windowHeight, int scale){
	SDL_FPoint* points = (SDL_FPoint*)SDL_malloc(sizeof(SDL_FPoint) * (windowWidth));
	int error;
	double x_val;
	te_variable vars[] = {{"x", &x_val}};
	te_expr *expr = te_compile(formula, vars, 1, &error);

	float centerX = (float)windowWidth / 2.0f;
	float centerY = (float)windowHeight / 2.0f;

	for (int i = 0; i < windowWidth; i++){
		SDL_FPoint point;
		float screenX = (float)i;
		float mathX = (screenX - centerX) / scale;
		point.x = screenX ;

		if (expr){
			x_val = (double)mathX;
			float mathY = (float)te_eval(expr);
			point.y = centerY - (mathY * scale);
		}
		else {
			point.y = centerY;
		}
		
		points[i] = point;
	}

	te_free(expr);
	if (points == NULL) return NULL;

	return points;
}

void CreateFunction(AppData* data, Function* function, Uint8 r, Uint8 g, Uint8 b){

	int windowWidth, windowHeight;
	SDL_GetWindowSize(window, &windowWidth, &windowHeight);

	function->points = GetPoints(function->formula, windowWidth, windowHeight, data->scale);

	function->colorR = r;
	function->colorG = g;
	function->colorB = b;

	appendArray(&data->formulas, function);

}

SDL_AppResult SDL_AppInit(void **appstate, int argc, char* argv[]){ 
	if(!SDL_Init(SDL_INIT_VIDEO)){ SDL_Log("Error inicializando sdl: %s", SDL_GetError());
		return SDL_APP_FAILURE;
	}
	if(!SDL_CreateWindowAndRenderer("GeoMati", 800, 600, SDL_WINDOW_RESIZABLE, &window, &renderer)){
		SDL_Log("Error inicializando ventana y renderer: %s", SDL_GetError());
		return SDL_APP_FAILURE;
	}
	AppData *data = (AppData*)SDL_calloc(1, sizeof(AppData)); SDL_GetWindowSize(window, &data->windowWidth, &data->windowHeight);

	if (argc > 1){
		data->formula = argv[1];

	}
	else{
		SDL_Log("No se especificó ninguna fórmula");
		*appstate = data;
		return SDL_APP_FAILURE;
	}
	
	initArray(&data->formulas, sizeof(Function), 2);
	Function function;
	function.formula = data->formula;
	function.points = NULL;
	
	CreateFunction(data, &function, 0, 0, 0);


	data->lastWindowWidth = 0;
	data->lastWindowHeight = 0;
	data->scale = 50;

	*appstate = data;

	return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event){
	AppData* data = appstate;
	if (event->type == SDL_EVENT_QUIT){
		return SDL_APP_SUCCESS;
	}
	if (event->type == SDL_EVENT_WINDOW_RESIZED){
		SDL_GetWindowSize(window, &data->windowWidth, &data->windowHeight);
		appstate = data;
	}

	return SDL_APP_CONTINUE;
}


SDL_AppResult SDL_AppIterate(void *appstate){
	AppData* data = appstate;
	
	// limpiar pantalla
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE_FLOAT);
	SDL_RenderClear(renderer);

	SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
	SDL_RenderLine(renderer, 0, (float)data->windowHeight/2, data->windowWidth, (float)data->windowHeight/2); // Eje X
	SDL_RenderLine(renderer, (float)data->windowWidth/2, 0, (float)data->windowWidth/2, data->windowHeight); // Eje Y
	
	for (int i = 0; i < data->formulas.size; i++){
		Function* function = (Function*)getArray(&data->formulas, i);
		if (function == NULL) continue;
		
		if (data->lastWindowWidth != data->windowWidth || data->lastWindowHeight != data->windowHeight){
			SDL_free(function->points);

			function->points = GetPoints(function->formula, data->windowWidth, data->windowHeight, data->scale);
			SDL_SetRenderDrawColor(renderer, function->colorR, function->colorG, function->colorB, SDL_ALPHA_OPAQUE_FLOAT);
		}

		SDL_RenderLines(renderer, function->points, data->windowWidth);
	}


	// Mostrar en pantalla
	SDL_RenderPresent(renderer);

	data->lastWindowWidth = data->windowWidth;
	data->lastWindowHeight = data->windowHeight;

	appstate = data;
	return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void* appstate, SDL_AppResult result){
	AppData* data = appstate;

	for (int i = 0; i < data->formulas.size; i++){
		Function* sin = (Function*)getArray(&data->formulas, i);

		if (sin->points != NULL){
			SDL_free(sin->points);
		}
	}
	SDL_free(data->formulas.data);
	SDL_free(data);


}
