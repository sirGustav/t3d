#include "core/aabb.h"
#include "core/axisangle.h"
#include "core/draw.h"
#include "core/filesystem.h"
#include "core/filesystemimagegenerator.h"
#include "core/mat4.h"
#include "core/os.h"
#include "core/path.h"
#include "core/random.h"
#include "core/shufflebag.h"
#include "core/texturetypes.h"
#include "core/str.h"
#include "core/mat4.h"

#include "render/camera.h"
#include "render/defaultfiles.h"
#include "render/materialshadercache.h"
#include "render/shaderattribute3d.h"
#include "render/texture.h"
#include "render/viewport.h"
#include "render/world.h"
#include <render/compiledmesh.h>
#include <render/debuggl.h>
#include <render/init.h>
#include <render/materialshader.h>
#include <render/texturecache.h>

#include "window/filesystem.h"
#include "window/fpscontroller.h"
#include "window/imgui.h"
#include "window/imguilibrary.h"
#include "window/sdlglcontext.h"
#include "window/sdllibrary.h"
#include "window/sdlwindow.h"
#include "window/timer.h"

#include "imgui/imgui.h"
#include <SDL.h>
#include <iostream>
#include <memory>

#include "t3d/tilelibrary.h"

int
main(int argc, char** argv)
{
  SdlLibrary sdl;
  if(sdl.ok == false)
  {
    return -1;
  }

  int width  = 1280;
  int height = 720;

  SdlWindow window{"t3d", width, height};
  if(window.window == nullptr)
  {
    return -1;
  }

  SdlGlContext context{&window};

  if(context.context == nullptr)
  {
    return -1;
  }

  Init init{SDL_GL_GetProcAddress};
  if(!init.ok)
  {
    return -4;
  }

  SetupOpenglDebug();

  const auto current_directory = GetCurrentDirectory();
  const auto base_path         = GetBasePath();
  const auto pref_path         = GetPrefPath();

  ImguiLibrary imgui{window.window, pref_path};

  Viewport viewport{
      Recti::FromWidthHeight(width, height).SetBottomLeftToCopy(0, 0)};
  viewport.Activate();

  FileSystem file_system;
  auto       catalog = FileSystemRootCatalog::AddRoot(&file_system);

  FileSystemRootFolder::AddRoot(&file_system, current_directory);
  FileSystemImageGenerator::AddRoot(&file_system, "img-plain");

  SetupDefaultFiles(catalog);

  MaterialShaderCache material_shader_cache{&file_system};

  // SET_ENUM_VALUES(TextureType, SetupTextureNames);
  SET_ENUM_FROM_FILE(&file_system, "texture_types.json", TextureType);

  TextureCache texture_cache{&file_system};

  TileLibrary tile_library;
  tile_library.AddDirectory("nature", &material_shader_cache, &texture_cache);

  World world;

  std::vector<std::shared_ptr<Actor>> actors;

  for(auto tile : tile_library.tiles)
  {
    auto actor = std::make_shared<Actor>(tile->mesh);
    world.AddActor(actor);
    actors.emplace_back(actor);
  }

  bool running = true;

  SdlTimer timer;


  bool capturing_mouse_movement = false;

  Camera camera;
  camera.SetPosition(vec3f(0, 0, 0));

  FpsController fps;
  fps.SetPosition(vec3f(0, 0, 3));

  while(running)
  {
    const bool  show_imgui = !capturing_mouse_movement;
    const float delta      = timer.Update();

    if(show_imgui)
    {
      imgui.Begin();

      ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiSetCond_FirstUseEver);

      ImGui::SetNextWindowSize(ImVec2(200, 100), ImGuiSetCond_FirstUseEver);
      ImGui::Begin("Light");
      ImGui::Combo(
          "Type",
          reinterpret_cast<int*>(world.light.GetTypeMod()),
          "Directional\0Point\0Spot\0\0");
      ImGui::ColorEdit3("Ambient", world.light.ModifyAmbient()->GetData());
      ImGui::ColorEdit3("Diffuse", world.light.ModifyDiffuse()->GetData());
      ImGui::ColorEdit3("Specular", world.light.ModifySpecular()->GetData());

      ImguiAngleSlider(
          "Cutoff Angle Inner", world.light.GetCutoffAngleInnerMod(), 0, 45);
      ImguiAngleSlider(
          "Cutoff Angle Outer", world.light.GetCutoffAngleOuterMod(), 0, 90);

      ImGui::End();

      ImGui::ShowTestWindow();


      // ImGui::ListBox("", &selection[i], items, IM_ARRAYSIZE(items));
      ImGui::Begin("Tiles");
      ImGui::ListBoxHeader("Tiles");

      for(auto tile : tile_library.tiles)
      {
        std::string display = Str{} << tile->name << ": "
                                    << tile->aabb.GetSize();
        ImGui::Selectable(display.c_str());
      }

      ImGui::ListBoxFooter();
      ImGui::End();
    }

    int items_per_row = 5;
    int col           = 0;
    int row           = 0;

    float size = 3;

    for(auto a : actors)
    {
      const auto x = col * size;
      const auto z = row * size;
      a->SetPosition(vec3f{x, 0, z});

      col += 1;
      if(col > items_per_row)
      {
        col = 0;
        row += 1;
      }
    }

    SDL_Event e;
    while(SDL_PollEvent(&e) != 0)
    {
      if(show_imgui)
      {
        imgui.ProcessEvents(&e);
      }
      switch(e.type)
      {
        case SDL_QUIT:
          running = false;
          break;
        case SDL_MOUSEMOTION:
          if(capturing_mouse_movement)
          {
            fps.Look(e.motion.xrel, e.motion.yrel);
          }
          break;
        case SDL_KEYDOWN:
        case SDL_KEYUP:
        {
          const bool down = e.type == SDL_KEYDOWN;

          fps.HandleSdlKey(e.key.keysym.sym, down);

          switch(e.key.keysym.sym)
          {
            case SDLK_ESCAPE:
              if(down)
              {
                running = false;
              }
              break;
            case SDLK_TAB:
              if(!down)
              {
                capturing_mouse_movement = !capturing_mouse_movement;
                window.KeepWithin(capturing_mouse_movement);
              }
              break;
            default:
              // ignore other keys
              break;
          }
        }
        break;
        default:
          // ignore other events
          break;
      }
    }

    fps.Update(delta);
    camera.SetPosition(fps.GetPosition());
    camera.SetRotation(fps.GetRotation());

    init.ClearScreen(Rgb::From(Color::DarkslateGray));
    world.Render(viewport, camera);

    if(show_imgui)
    {
      imgui.Render();
    }

    SDL_GL_SwapWindow(window.window);
  }

  return 0;
}
