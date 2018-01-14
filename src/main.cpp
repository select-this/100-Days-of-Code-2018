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
#include <random>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <vector>

//----------------------------------------------------------------------------------------------------------------------

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
    int percent = 1;
};

namespace std
{
    template<>
    struct hash<bar>
    {
        std::size_t operator()(const bar& rhs) const
        {
            return rhs.id;
        }
    };
}

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
        constexpr int WINDOW_FLAGS = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN/* | SDL_WINDOW_RESIZABLE*/;
        constexpr const char* WINDOW_TITLE = "100 Days of Code 2018";

        const std::unique_ptr<SDL_Window, void(*)(SDL_Window*)> window(SDL_CreateWindow(
            WINDOW_TITLE, WINDOW_HPOS, WINDOW_VPOS, WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_FLAGS), SDL_DestroyWindow);

        if(!window)
        {
            throw std::runtime_error(SDL_GetError());
        }

        const std::unique_ptr<void, void(*)(SDL_GLContext)> context(
            SDL_GL_CreateContext(window.get()), SDL_GL_DeleteContext);

        if(!context)
        {
            throw std::runtime_error(SDL_GetError());
        }

        const std::unique_ptr<SDL_Renderer, void(*)(SDL_Renderer*)> renderer(
            SDL_CreateRenderer(window.get(), -1,  SDL_RENDERER_ACCELERATED |
                SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_TARGETTEXTURE), SDL_DestroyRenderer);

        if(!renderer)
        {
            throw std::runtime_error(SDL_GetError());
        }

        const std::unique_ptr<TTF_Font, void(*)(TTF_Font*)> font(
            TTF_OpenFont("../asset/font/saxmono.ttf", 16), TTF_CloseFont);

        if(!font)
        {
            throw std::runtime_error(SDL_GetError());
        }

        constexpr const char* key_string =
            "Press x key to start / stop depletion of bar, z key to refill bar, q key to quit";

        const std::unique_ptr<SDL_Surface, void(*)(SDL_Surface*)> key_string_surface(
            TTF_RenderText_Solid(font.get(), key_string, { 255, 255, 255, 255 }), SDL_FreeSurface);

        if(!key_string_surface)
        {
            throw std::runtime_error(SDL_GetError());
        }

        const std::unique_ptr<SDL_Texture, void(*)(SDL_Texture*)> key_string_texture(
            SDL_CreateTextureFromSurface(renderer.get(), key_string_surface.get()), SDL_DestroyTexture);

        if(!key_string_texture)
        {
            throw std::runtime_error(SDL_GetError());
        }

        int key_string_width = 0;
        int key_string_height = 0;

        SDL_QueryTexture(key_string_texture.get(), NULL, NULL, &key_string_width, &key_string_height);

        SDL_Rect key_string_rect = { 10, 10, key_string_width, key_string_height };

        constexpr const char* frames_string =
            "Average frames per second: ";

        const std::unique_ptr<SDL_Surface, void(*)(SDL_Surface*)> frames_string_surface(
            TTF_RenderText_Solid(font.get(), frames_string, { 255, 255, 255, 255 }), SDL_FreeSurface);

        if(!frames_string_surface)
        {
            throw std::runtime_error(SDL_GetError());
        }

        const std::unique_ptr<SDL_Texture, void(*)(SDL_Texture*)> frames_string_texture(
            SDL_CreateTextureFromSurface(renderer.get(), frames_string_surface.get()), SDL_DestroyTexture);

        if(!frames_string_texture)
        {
            throw std::runtime_error(SDL_GetError());
        }

        int frames_string_width = 0;
        int frames_string_height = 0;

        SDL_QueryTexture(frames_string_texture.get(), NULL, NULL, &frames_string_width, &frames_string_height);

        SDL_Rect frames_string_rect = { 10, 20 + key_string_height, frames_string_width, frames_string_height };

        int renderer_width = 0;
        int renderer_height = 0;

        SDL_GetRendererOutputSize(renderer.get(), &renderer_width, &renderer_height);

        SDL_Event event;

        bool quit = false;

        std::list<bar> bars({{ 0, 90, 0, 100 }});
        std::map<std::reference_wrapper<bar>, depletion, std::less<bar>> depletions;

        const SDL_Rect outer_rect
        {
            static_cast<int>(std::floor(renderer_width / 2 - 200 + 0.5)),
            static_cast<int>(std::floor(renderer_height / 2 - 40 + 0.5)),
            400,
            80
        };

        SDL_Rect inner_rect
        {
            outer_rect.x + 10,
            outer_rect.y + 10,
            outer_rect.w - 10,
            outer_rect.h - 20
        };

        int frames = 0;

        double start_time = SDL_GetTicks();
        double last_frame_time = 0.0;
        double cycles_left_over = 0.0;
        double average_frames_per_second = 0.0;

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

                            case SDLK_q:
                                quit = true;
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

            double current_time = SDL_GetTicks();
            double update_iterations = ((current_time - last_frame_time) + cycles_left_over);

            if(update_iterations > (MAX_CYCLES_PER_FRAME * UPDATE_INTERVAL))
            {
                update_iterations = (MAX_CYCLES_PER_FRAME * UPDATE_INTERVAL);
            }

            while(update_iterations > UPDATE_INTERVAL)
            {
                update_iterations -= UPDATE_INTERVAL;

                for(auto iterator = depletions.begin(); iterator != depletions.end(); ++iterator)
                {
                    iterator->first.get().percent_current -= iterator->second.percent;

                    if(iterator->first.get().percent_current < iterator->first.get().percent_min)
                    {
                        iterator = depletions.erase(iterator);

                        if(iterator == depletions.end())
                        {
                            break;
                        }
                    }
                }
            }

            average_frames_per_second = frames / ((SDL_GetTicks() - start_time) / 1000.0);

            cycles_left_over = update_iterations;
            last_frame_time = current_time;

            if(average_frames_per_second > 2000000)
            {
                average_frames_per_second = 0;
            }

            const std::unique_ptr<SDL_Surface, void(*)(SDL_Surface*)> afps_string_surface(
                TTF_RenderText_Solid(font.get(),
                    std::to_string(average_frames_per_second).c_str(), { 255, 255, 255, 255 }), SDL_FreeSurface);

            if(!afps_string_surface)
            {
                throw std::runtime_error(SDL_GetError());
            }

            const std::unique_ptr<SDL_Texture, void(*)(SDL_Texture*)> afps_string_texture(
                SDL_CreateTextureFromSurface(renderer.get(), afps_string_surface.get()), SDL_DestroyTexture);

            if(!afps_string_texture)
            {
                throw std::runtime_error(SDL_GetError());
            }

            int afps_string_width = 0;
            int afps_string_height = 0;

            SDL_QueryTexture(afps_string_texture.get(), NULL, NULL, &afps_string_width, &afps_string_height);

            SDL_Rect afps_string_rect = {
                10 + frames_string_width, 20 + key_string_height, afps_string_width, afps_string_height };

            SDL_SetRenderDrawColor(renderer.get(), 0, 0, 0, 255);

            SDL_RenderClear(renderer.get());

            SDL_RenderCopy(renderer.get(), key_string_texture.get(), nullptr, &key_string_rect);
            SDL_RenderCopy(renderer.get(), frames_string_texture.get(), nullptr, &frames_string_rect);
            SDL_RenderCopy(renderer.get(), afps_string_texture.get(), nullptr, &afps_string_rect);

            SDL_SetRenderDrawColor(renderer.get(), 50, 100, 200, 255);

            SDL_RenderFillRect(renderer.get(), &outer_rect);

            inner_rect.w = static_cast<int>(std::floor(
                380 * bars.front().percent_current / bars.front().percent_max + 0.5));

            if(inner_rect.w > 0)
            {
                SDL_SetRenderDrawColor(renderer.get(), 20, 70, 170, 255);

                SDL_RenderFillRect(renderer.get(), &inner_rect);
            }

            SDL_RenderPresent(renderer.get());

            ++frames;
        }
    }
    catch(const std::runtime_error& exception)
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
