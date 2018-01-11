//----------------------------------------------------------------------------------------------------------------------
//
// Copyright(c) 2018 Darren Paul Ferrie
//
// Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated
// documentation files(the "Software"), to deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to
// permit persons to whom the Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all copies or substantial portions of the
// Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
// WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
// COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
// OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//
//----------------------------------------------------------------------------------------------------------------------

#include <cmath>
#include <cstdint>
#include <exception>
#include <iostream>
#include <memory>
#include <SDL2/SDL.h>

//----------------------------------------------------------------------------------------------------------------------

int main()
{
    try
    {
        if(SDL_Init(SDL_INIT_VIDEO) != 0 ||
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3) != 0 ||
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3) != 0 ||
            SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1) != 0 ||
            SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24) != 0)
        {
            throw std::runtime_error(SDL_GetError());
        }

        constexpr int WINDOW_WIDTH = 1024;
        constexpr int WINDOW_HEIGHT = 768;
        constexpr int WINDOW_HPOS = SDL_WINDOWPOS_UNDEFINED;
        constexpr int WINDOW_VPOS = SDL_WINDOWPOS_UNDEFINED;
        constexpr int WINDOW_FLAGS = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN;
        constexpr const char* WINDOW_TITLE = "100 Days of Code 2018";

        std::unique_ptr<SDL_Window, void(*)(SDL_Window*)> window(SDL_CreateWindow(
            WINDOW_TITLE, WINDOW_HPOS, WINDOW_VPOS, WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_FLAGS), SDL_DestroyWindow);

        if(!window)
        {
            throw std::runtime_error(SDL_GetError());
        }

        std::unique_ptr<void, void(*)(SDL_GLContext)> context(SDL_GL_CreateContext(window.get()), SDL_GL_DeleteContext);

        if(!context)
        {
            throw std::runtime_error(SDL_GetError());
        }

        std::unique_ptr<SDL_Renderer, void(*)(SDL_Renderer*)> renderer(
            SDL_CreateRenderer(window.get(), -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC),
            SDL_DestroyRenderer);

        if(!renderer)
        {
            throw std::runtime_error(SDL_GetError());
        }

        SDL_Event event;

        bool quit = false;

        while(!quit)
        {
            while(SDL_PollEvent(&event))
            {
                switch(event.type)
                {
                    case SDL_QUIT:
                        quit = true;
                        break;

                    default:
                        break;
                }
            }

            SDL_SetRenderDrawColor(renderer.get(), 0, 0, 0, 255);

            SDL_RenderClear(renderer.get());

            int renderer_width = 0;
            int renderer_height = 0;

            SDL_GetRendererOutputSize(renderer.get(), &renderer_width, &renderer_height);

            SDL_Rect outer_rect
            {
                static_cast<int>(std::floor(renderer_width / 2 - 200 + 0.5)),
                static_cast<int>(std::floor(renderer_height / 2 - 70 + 0.5)),
                400,
                140
            };

            SDL_SetRenderDrawColor(renderer.get(), 50, 100, 200, 255);

            SDL_RenderFillRect(renderer.get(), &outer_rect);

            SDL_Rect inner_rect
            {
                static_cast<int>(std::floor(renderer_width / 2 - 190 + 0.5)),
                static_cast<int>(std::floor(renderer_height / 2 - 60 + 0.5)),
                380,
                120
            };

            SDL_SetRenderDrawColor(renderer.get(), 20, 70, 170, 255);

            SDL_RenderFillRect(renderer.get(), &inner_rect);

            SDL_RenderPresent(renderer.get());
        }
    }
    catch(std::runtime_error& exception)
    {
        std::cout << "Oh no, something bad happened! Exception: " << exception.what() << std::endl;
    }

    SDL_Quit();

    return 0;
}

//----------------------------------------------------------------------------------------------------------------------
