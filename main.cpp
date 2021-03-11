
#include <iostream>
#include <sstream>
#include <dirent.h>
#include <SFML/Graphics.hpp>
using namespace std;
using namespace sf;

#define WIDTH 800
#define HEIGHT 600

RenderWindow window(VideoMode(WIDTH,HEIGHT),"highresimageviewer", Style::Default);

#define IMGQUEUESIZ 6

string imgtitle[IMGQUEUESIZ];
Texture texture[IMGQUEUESIZ];
Sprite sprite[IMGQUEUESIZ];
bool loaded[IMGQUEUESIZ];
int index=0;

Mutex mutex;


#define zoomindexmax 10
float zoomrange[zoomindexmax] = {
//   0.5,
//   0.6,
//   0.7,
//   0.8,
//   0.9,

   1.0,
   1.1,
   1.2,
   1.3,
   1.4,

   1.9,
   2.5,
   3.0,
   4.0,
   5.0,
};

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
            cout << "load " << imgindex << " - " << file_list[imgindex] << endl;

            loaded[i] = true;
            imgtitle[i] = file_list[imgindex];
            texture[i] = Texture();
            texture[i].loadFromFile(file_list[imgindex]);
            sprite[i] = Sprite();
            sprite[i].setTexture(texture[i]);
            sprite[i].setOrigin(texture[i].getSize().x/2-window.getSize().x/2,texture[i].getSize().y/2-window.getSize().y/2);
            sprite[i].setPosition(0,0);

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

   window.setVerticalSyncEnabled(true);

   int sx=0,sy=0;
   int dx=0,dy=0;
   int currx=0,curry=0;
   bool moving = false;
   int zoomlevel=0;

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
               mutex.lock();
               if(loaded[index])
               {
                  loaded[index] = false;
                  index++;
                  if(index>IMGQUEUESIZ-1)index=0;
               }
               mutex.unlock();
            }
            break;
         case Event::MouseButtonPressed:
            if(e.mouseButton.button == Mouse::Left)
            {
               moving=true;
               sx = Mouse::getPosition().x;
               sy = Mouse::getPosition().y;
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
               zoomlevel++;
               if(zoomlevel>=zoomindexmax)
                  zoomlevel=zoomindexmax-1;
            }
            else
            {
               if(zoomlevel>0)
                  zoomlevel--;
            }
            break;
         }
      }

      if(moving)
      {
         dx = (sx - Mouse::getPosition().x)*zoomrange[zoomlevel] + (currx);
         dy = (sy - Mouse::getPosition().y)*zoomrange[zoomlevel] + (curry);
      }

//      int picpos_win_relative_x = dx - ((int)texture[index].getSize().x/2) + (int)texture[index].getSize().x - ((int)window.getSize().x/2)*zoomrange[zoomlevel] ;
//      int picpos_win_relative_y = dy - ((int)texture[index].getSize().y/2) + (int)texture[index].getSize().y - ((int)window.getSize().y/2)*zoomrange[zoomlevel] ;
//      cout << picpos_win_relative_x << ", " << picpos_win_relative_y << endl;
//      cout << texture[index].getSize().x/zoomrange[zoomlevel] << ", " << texture[index].getSize().y/zoomrange[zoomlevel] << endl;


      window.clear(Color::Black);

      if(loaded[index])
      {
         View imgview = window.getDefaultView();
         imgview.setSize(winsizechangedwid,winsizechangedhei);
         imgview.zoom(zoomrange[zoomlevel]);
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
         ss << zoomrange[zoomlevel];
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
