
#include <iostream>
#include <SFML/Graphics.hpp>
using namespace std;
using namespace sf;

Texture t2[4];
Sprite s[4];
bool loaded[5];

string fnames[4] = {
   "tyuleniy_oli_2016097_lrg.jpg",
   "tyuleniy_oli_2016097_lrg.jpg",
   "tyuleniy_oli_2016097_lrg.jpg",
   "tyuleniy_oli_2016097_lrg.jpg",
};


RenderWindow window(VideoMode(1600,900),"highresimageviewer", Style::Default);

void runner()
{
   for(int i=0;i<4;i++)
   {
      t2[i].loadFromFile(fnames[i]);
      loaded[i] = true;
      s[i].setTexture(t2[i]);
      s[i].setOrigin(0,0);
      s[i].setPosition(0,0);

      cout << i  << " loaded "<< endl;
   }
}

int main()
{
   window.setVerticalSyncEnabled(true);

   int sx=0,sy=0;
   int dx=0,dy=0;
   int currx=0,curry=0;
   bool moving = false;
   float zoomlevel=10.0;

   View view(FloatRect(0,0,window.getSize().x,window.getSize().y));
   view.setCenter(Vector2f(0,0));
   view.zoom(zoomlevel);
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
               if(zoomlevel<10.0) zoomlevel*=1.1;
            }
            else
            {
               if(zoomlevel>0.5) zoomlevel*=0.9;
            }
            break;
         }
      }

      if(moving)
      {
         dx = (sx - Mouse::getPosition().x)*zoomlevel + (currx);
         dy = (sy - Mouse::getPosition().y)*zoomlevel + (curry);
      }

      cout << dx << ", " << dy << endl;

      window.clear();

      View v = window.getDefaultView();
      v.setSize(winsizechangedwid,winsizechangedhei);
      v.zoom(zoomlevel);
      v.move(Vector2f(dx,dy));
      window.setView(v);

      for(int i=0;i<4;i++)
      {
         if(loaded[i])
         {
            s[i].setOrigin(0,0);
            s[i].setPosition((i%2)*t2[i].getSize().x, (i/2)*t2[i].getSize().y);
            window.draw(s[i]);
         }
      }

      window.display();
   }


   return 0;
}
