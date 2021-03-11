
#include <iostream>
#include <SFML/Graphics.hpp>
using namespace std;
using namespace sf;

RenderWindow window(VideoMode(1600,900),"highresimageviewer", Style::Default);

Texture texture[4];
Sprite sprite[4];
bool loaded[4];
int index=0;

float zoomrange[20] = {
   1.0,
   1.1,
   1.2,
   1.3,
   1.4,

   2.0,
   2.5,
   3.0,
   4.0,
   5.0,
};

string fnames[4] = {
   "99.jpg",
   "tyuleniy_oli_2016097_lrg.jpg",
   "99.jpg",
   "tyuleniy_oli_2016097_lrg.jpg",
};


void runner()
{
   while(window.isOpen())
   {
      for(int i=0;i<4;i++)
      {
         if(!loaded[i])
         {
            texture[i].loadFromFile(fnames[i]);
            loaded[i] = true;
            sprite[i].setTexture(texture[i]);
            sprite[i].setOrigin(texture[i].getSize().x/2-window.getSize().x/2,texture[i].getSize().y/2-window.getSize().y/2);
            sprite[i].setPosition(0,0);

            cout << i  << " loaded "<< endl;
         }
      }
   }
}

int main()
{
   window.setVerticalSyncEnabled(true);

   int sx=0,sy=0;
   int dx=0,dy=0;
   int currx=0,curry=0;
   bool moving = false;
   int zoomlevel=0;

   View view(FloatRect(0,0,window.getSize().x,window.getSize().y));
   view.setCenter(Vector2f(0,0));
   view.zoom(zoomrange[zoomlevel]);
   window.setView(view);

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
               if(loaded[index])
               {
                  loaded[index] = false;
                  index++;
                  if(index>3)index=0;
               }
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
               if(zoomlevel>=10)
                  zoomlevel=9;
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

      cout << dx - ((int)texture[index].getSize().x/2) + (int)texture[index].getSize().x - ((int)window.getSize().x/2)*zoomrange[zoomlevel] << ", " <<
              dy - ((int)texture[index].getSize().y/2) + (int)texture[index].getSize().y - ((int)window.getSize().y/2)*zoomrange[zoomlevel] << endl;

      window.clear();
      View v = window.getDefaultView();
      v.setSize(winsizechangedwid,winsizechangedhei);
      v.zoom(zoomrange[zoomlevel]);
      v.move(Vector2f(dx,dy));
      window.setView(v);

      if(loaded[index])
      {
         window.draw(sprite[index]);
      }

      window.display();
   }

   imgdynamicloadrunner.wait();

   return 0;
}
