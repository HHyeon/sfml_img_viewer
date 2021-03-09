#include <iostream>
#include <SFML/Graphics.hpp>
using namespace std;
using namespace sf;

int main()
{
   RenderWindow window(VideoMode(800,600),"highresimageviewer", Style::Default);
   window.setVerticalSyncEnabled(true);

   Texture t1,t2;
   t1.loadFromFile("99.jpg");
   t2.loadFromFile("tyuleniy_oli_2016097_lrg.jpg");

   cout << "w:" << t1.getSize().x << endl;
   cout << "h:" << t1.getSize().y << endl;
   Sprite s(t1);

   int sx=0,sy=0;
   int dx=0,dy=0;
   int currx=0,curry=0;
   bool moving = false;
   float zoomlevel=1.0, zoomlevel_past=0.0;

   View view(FloatRect(0,0,800,600));
   view.setCenter(Vector2f(0,0));
   view.zoom(zoomlevel);
   window.setView(view);

   Event e;
   while(window.isOpen())
   {
      while(window.pollEvent(e))
      {
         if(e.type == Event::Closed)
         {
            window.close();
         }
         else if(e.type == Event::KeyPressed)
         {
            if(e.key.code == Keyboard::Q)
            {
               window.close();
            }
         }
         else if(e.type == Event::MouseButtonPressed)
         {
            if(e.mouseButton.button == Mouse::Left)
            {
               sx=Mouse::getPosition().x ;
               sy=Mouse::getPosition().y;
               moving=true;
            }
         }
         else if(e.type == Event::MouseButtonReleased)
         {
            if(e.mouseButton.button == Mouse::Left)
            {
               sx=sy=-1;
               currx = dx;
               curry = dy;
               moving = false;
            }
         }
         else if(e.type == Event::MouseWheelMoved)
         {
            if(e.mouseWheel.delta>0)
            {
               if(zoomlevel<3)
                  zoomlevel++;
            }
            else
            {
               if(zoomlevel>1)
                  zoomlevel--;
            }
         }
      }

      window.clear();

      if(moving || (zoomlevel != zoomlevel_past))
      {
         int pushx = (currx/zoomlevel);
         int pushy = (curry/zoomlevel);

         if(zoomlevel != zoomlevel_past)
         {
            pushx = pushy = 0;
         }


         dx = sx - Mouse::getPosition().x + pushx;
         dy = sy - Mouse::getPosition().y + pushy;

         cout << sx << ", " << sy << " - " << dx << ", " << dy << endl;

         dx*=zoomlevel;
         dy*=zoomlevel;

         View v = window.getDefaultView();
         v.zoom(zoomlevel);
         v.move(Vector2f(dx,dy));
         window.setView(v);

         zoomlevel_past = zoomlevel;
      }

      window.draw(s);
      window.display();
   }


   return 0;
}
