
#include <iostream>
#include <SFML/Graphics.hpp>
using namespace std;
using namespace sf;

int main()
{
   RenderWindow window(VideoMode(1600,900),"highresimageviewer", Style::Default);
   window.setVerticalSyncEnabled(true);

   Texture t1,t2;
   t1.loadFromFile("99.jpg");
//   t2.loadFromFile("tyuleniy_oli_2016097_lrg.jpg");

   cout << "w:" << t1.getSize().x << endl;
   cout << "h:" << t1.getSize().y << endl;
   Sprite s(t1);
   s.setOrigin(t1.getSize().x/2-window.getSize().x,t1.getSize().y/2-window.getSize().y);

   int sx=0,sy=0;
   int dx=0,dy=0;
   int currx=0,curry=0;
   bool moving = false;
   float zoomlevel=1.0, zoomlevel_past=1.0;

   View view(FloatRect(0,0,window.getSize().x,window.getSize().y));
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
               moving=true;
               sx = Mouse::getPosition().x;
               sy = Mouse::getPosition().y;
            }
         }
         else if(e.type == Event::MouseButtonReleased)
         {
            if(e.mouseButton.button == Mouse::Left)
            {
               moving = false;
               currx = dx;
               curry = dy;
            }
         }
         else if(e.type == Event::MouseWheelMoved)
         {
            if(e.mouseWheel.delta<0)
            {
               if(zoomlevel<5.0) zoomlevel*=1.5;
            }
            else
            {
               if(zoomlevel>0.5) zoomlevel*=0.5;
            }
         }
      }

      if(moving)
      {
         dx = (sx - Mouse::getPosition().x)*zoomlevel + (currx);
         dy = (sy - Mouse::getPosition().y)*zoomlevel + (curry);
      }

//      cout << dx << ", " << dy << ", " << zoomlevel << endl;

      window.clear();
      View v = window.getDefaultView();
      v.zoom(zoomlevel);
      v.move(Vector2f(dx,dy));
      window.setView(v);
      window.draw(s);
      window.display();
   }


   return 0;
}
