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

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <exception>
#include <iostream>
#include <list>
#include <memory>
#include <map>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

//----------------------------------------------------------------------------------------------------------------------

int main()
{
    try
    {
        if(SDL_Init(SDL_INIT_VIDEO) != 0 ||
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3) != 0 ||
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3) != 0 ||
            SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1) != 0 ||
            SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24) != 0 ||
            TTF_Init() != 0)
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

        std::unique_ptr<TTF_Font, void(*)(TTF_Font*)> font(
            TTF_OpenFont("../asset/font/saxmono.ttf", 16), TTF_CloseFont);

        if(!font)
        {
            throw std::runtime_error(SDL_GetError());
        }

        constexpr const char* text = "Press x key to start / stop depletion of bar, or z key to refill bar";

        std::unique_ptr<SDL_Surface, void(*)(SDL_Surface*)> text_surface(
            TTF_RenderText_Solid(font.get(), text, { 255, 255, 255, 255 }), SDL_FreeSurface);

        if(!text_surface)
        {
            throw std::runtime_error(SDL_GetError());
        }

        std::unique_ptr<SDL_Texture, void(*)(SDL_Texture*)> text_texture(
            SDL_CreateTextureFromSurface(renderer.get(), text_surface.get()), SDL_DestroyTexture);

        if(!text_texture)
        {
            throw std::runtime_error(SDL_GetError());
        }

        int text_width = 0;
        int text_height = 0;

        SDL_QueryTexture(text_texture.get(), NULL, NULL, &text_width, &text_height);

        SDL_Rect text_rect = { 10, 10, text_width, text_height };

        int renderer_width = 0;
        int renderer_height = 0;

        SDL_GetRendererOutputSize(renderer.get(), &renderer_width, &renderer_height);

        SDL_Event event;

        bool quit = false;

        struct bar
        {
            bool operator<(const bar& rhs) const
            {
                return id < rhs.id;
            }

            int id;
            int percent_current;
            int percent_min;
            int percent_max;
        };

        struct depletion
        {
            int percent;
        };

        std::list<bar> bars({{ 0, 90, 0, 100 }});
        std::map<std::reference_wrapper<bar>, depletion, std::less<bar>> depletions;

        while(!quit)
        {
            while(SDL_PollEvent(&event))
            {
                switch(event.type)
                {
                    case SDL_QUIT:
                        quit = true;
                        break;

                    case SDL_KEYDOWN:
                        switch(event.key.keysym.sym)
                        {
                            case SDLK_x:
                                if(depletions.empty())
                                {
                                    depletions[bars.front()] = { 1 };
                                }
                                else
                                {
                                    depletions.clear();
                                }
                                break;

                            case SDLK_z:
                                bars.front().percent_current = bars.front().percent_max;
                                break;
                        }
                        break;

                    default:
                        break;
                }
            }

          //constexpr int MAXIMUM_FRAME_RATE = 120;
            constexpr int MAXIMUM_FRAME_RATE = 30;
            constexpr int MINIMUM_FRAME_RATE = 15;
            constexpr double UPDATE_INTERVAL = 1.0 / MAXIMUM_FRAME_RATE;
            constexpr double MAX_CYCLES_PER_FRAME = MAXIMUM_FRAME_RATE / MINIMUM_FRAME_RATE;

            static double lastFrameTime = 0.0;
            static double cyclesLeftOver = 0.0;

            double currentTime = SDL_GetTicks();
            double updateIterations = ((currentTime - lastFrameTime) + cyclesLeftOver);

            if(updateIterations > (MAX_CYCLES_PER_FRAME * UPDATE_INTERVAL))
            {
                updateIterations = (MAX_CYCLES_PER_FRAME * UPDATE_INTERVAL);
            }

            while(updateIterations > UPDATE_INTERVAL)
            {
                updateIterations -= UPDATE_INTERVAL;

                for (auto iterator = depletions.begin(); iterator != depletions.end(); ++iterator)
                {
                    iterator->first.get().percent_current -= iterator->second.percent;

                    if (iterator->first.get().percent_current < iterator->first.get().percent_min)
                    {
                        iterator = depletions.erase(iterator);

                        if (iterator == depletions.end())
                        {
                            break;
                        }
                    }
                }
            }

            cyclesLeftOver = updateIterations;
            lastFrameTime = currentTime;

            SDL_SetRenderDrawColor(renderer.get(), 0, 0, 0, 255);

            SDL_RenderClear(renderer.get());

            SDL_RenderCopy(renderer.get(), text_texture.get(), nullptr, &text_rect);

            SDL_Rect outer_rect
            {
                static_cast<int>(std::floor(renderer_width / 2 - 200 + 0.5)),
                static_cast<int>(std::floor(renderer_height / 2 - 40 + 0.5)),
                400,
                80
            };

            SDL_SetRenderDrawColor(renderer.get(), 50, 100, 200, 255);

            SDL_RenderFillRect(renderer.get(), &outer_rect);

            int inner_width = static_cast<int>(
                std::floor(380 * bars.front().percent_current / bars.front().percent_max + 0.5));

            if (inner_width > 0)
            {
                SDL_Rect inner_rect
                {
                    static_cast<int>(std::floor(renderer_width / 2 - 190 + 0.5)),
                    static_cast<int>(std::floor(renderer_height / 2 - 30 + 0.5)),
                    inner_width,
                    60
                };

                SDL_SetRenderDrawColor(renderer.get(), 20, 70, 170, 255);

                SDL_RenderFillRect(renderer.get(), &inner_rect);
            }

            SDL_RenderPresent(renderer.get());
        }
    }
    catch(std::runtime_error& exception)
    {
        std::cout << "Oh no, something bad happened! Exception: " << exception.what() << std::endl;
    }

    if(TTF_WasInit())
    {
        TTF_Quit();
    }

    SDL_Quit();

    return 0;
}

//----------------------------------------------------------------------------------------------------------------------
