
#include <iostream>
#include <sstream>
#include <dirent.h>
#include <SFML/Graphics.hpp>
using namespace std;
using namespace sf;

#define WIDTH 1600
#define HEIGHT 900

RenderWindow window(VideoMode(WIDTH,HEIGHT),"highresimageviewer", Style::Default);

#define IMGQUEUESIZ 4
string imgtitle[IMGQUEUESIZ];
Texture texture[IMGQUEUESIZ];
Sprite sprite[IMGQUEUESIZ];
bool loaded[IMGQUEUESIZ];
int index=0;

Mutex mutex;

#define zoommin 1.0
#define zoommax 5

vector<string> file_list;

void runner()
{
   int imgindex=0;
   while(window.isOpen())
   {
      for(int i=0;i<IMGQUEUESIZ;i++)
      {
         if(!loaded[i])
         {
//            mutex.lock();
//            mutex.unlock();

            loaded[i] = true;
            imgtitle[i] = file_list[imgindex];
            texture[i] = Texture();
            texture[i].loadFromFile(file_list[imgindex]);
            sprite[i] = Sprite();
            sprite[i].setTexture(texture[i]);
            sprite[i].setOrigin(texture[i].getSize().x/2-window.getSize().x/2,texture[i].getSize().y/2-window.getSize().y/2);
            sprite[i].setPosition(0,0);

            cout << "load " << imgindex << " - " << file_list[imgindex] << endl;

            imgindex++;
            if(imgindex >= (int)file_list.size())
               imgindex=0;
         }

         if(!window.isOpen())
            break;
      }
      sleep(milliseconds(100));
   }
   cout << "load runner terminated\n";
}

void nextpicture()
{
   if(loaded[index])
   {
      loaded[index] = false;
      index++;
      if(index>IMGQUEUESIZ-1)index=0;
//      mutex.lock();
      cout << imgtitle[index] << endl;
//      mutex.unlock();
   }
}

int main()
{
   DIR *dir;
   struct dirent *dirread;

   string DIRNAME = "8kpics/";

   if((dir=opendir(DIRNAME.c_str()))!=nullptr)
   {
      while((dirread=readdir(dir))!=nullptr)
      {
         string item = string(dirread->d_name);
         if(item == "." || item == "..") continue;
         string name = DIRNAME + item;
         cout << name << endl;
         file_list.push_back(name);
      }
   }
   else
   {
      return 1;
   }

   window.setFramerateLimit(30);

   int sx=0,sy=0;
   int dx=0,dy=0;
   int currx=0,curry=0;
   bool moving = false;
   double zoomlevel=zoommin;

   Font font;
   font.loadFromFile("AdobeGothicStd-Bold.otf");
   Text text("loading", font, 30);
   text.setPosition(10,10);
   text.setOutlineColor(Color::Black);
   text.setOutlineThickness(1.0);
   Text titletext("", font, 30);
   titletext.setOutlineColor(Color::Black);
   titletext.setOutlineThickness(1.0);
   Text zoomtext("", font, 30);
   zoomtext.setOutlineColor(Color::Black);
   zoomtext.setOutlineThickness(1.0);

   int winsizechangedwid=window.getSize().x;
   int winsizechangedhei=window.getSize().y;

   Thread imgdynamicloadrunner(runner);
   imgdynamicloadrunner.launch();

   Event e;
   while(window.isOpen())
   {
      while(window.pollEvent(e))
      {
         switch((int)e.type)
         {
         case Event::Closed:
            window.close();
            break;
         case Event::Resized:
            {
               winsizechangedwid = e.size.width;
               winsizechangedhei = e.size.height;
            }
            break;
         case Event::KeyPressed:
            if(e.key.code == Keyboard::Q)
            {
               window.close();
            }
            else if(e.key.code == Keyboard::Space)
            {
               nextpicture();
            }
            break;
         case Event::MouseButtonPressed:
            if(e.mouseButton.button == Mouse::Left)
            {
               moving=true;
               sx = Mouse::getPosition().x;
               sy = Mouse::getPosition().y;
            }
            else if(e.mouseButton.button == Mouse::Middle)
            {
               window.close();
            }
            else if(e.mouseButton.button == Mouse::Right)
            {
               nextpicture();
            }
            break;

         case Event::MouseButtonReleased:
            if(e.mouseButton.button == Mouse::Left)
            {
               moving = false;
               currx = dx;
               curry = dy;
            }
            break;

         case Event::MouseWheelMoved:
            if(e.mouseWheel.delta<0)
            {
               zoomlevel*=1.1;
               if(zoomlevel > zoommax) zoomlevel = zoommax;
            }
            else
            {
               zoomlevel*=0.9;
               if(zoomlevel < zoommin) zoomlevel = zoommin;
            }
            break;
         }
      }

      if(moving)
      {
         dx = (sx - Mouse::getPosition().x)*zoomlevel + (currx);
         dy = (sy - Mouse::getPosition().y)*zoomlevel + (curry);
      }

      int picpos_win_relative_x = dx - ((int)texture[index].getSize().x/2) + (int)texture[index].getSize().x - ((int)window.getSize().x/2)*zoomlevel ;
      int picpos_win_relative_y = dy - ((int)texture[index].getSize().y/2) + (int)texture[index].getSize().y - ((int)window.getSize().y/2)*zoomlevel ;
      cout << picpos_win_relative_x << ", " << picpos_win_relative_y << endl;
//      cout << texture[index].getSize().x/zoomlevel << ", " << texture[index].getSize().y/zoomlevel << endl;

      window.clear(Color::Black);

      if(loaded[index])
      {
         View imgview = window.getDefaultView();
         imgview.setSize(winsizechangedwid,winsizechangedhei);
         imgview.zoom(zoomlevel);
         imgview.move(Vector2f(dx,dy));
         window.setView(imgview);
         window.draw(sprite[index]);

         imgview = window.getDefaultView();
         imgview.setSize(winsizechangedwid,winsizechangedhei);
         window.setView(imgview);


         titletext.setString(imgtitle[index]);
         titletext.setPosition(WIDTH/2 - (int)window.getSize().x/2 + 10, HEIGHT/2 - (int)window.getSize().y/2 + 10);
         window.draw(titletext);

         stringstream ss;
         ss << zoomlevel;
         zoomtext.setString(ss.str());
         zoomtext.setPosition(WIDTH/2 - (int)window.getSize().x/2 + 10, HEIGHT/2 - (int)window.getSize().y/2 + 50);
         window.draw(zoomtext);
      }
      else
      {
         View imgview = window.getDefaultView();
         imgview.setSize(winsizechangedwid,winsizechangedhei);
         window.setView(imgview);
         text.setPosition(WIDTH/2 - (int)window.getSize().x/2 + 10, HEIGHT/2 - (int)window.getSize().y/2 + 10);
         window.draw(text);
      }

      window.display();
   }

   imgdynamicloadrunner.wait();

   return 0;
}
