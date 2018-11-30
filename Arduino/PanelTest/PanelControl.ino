//
enum ModeList 
{
  AllButton,
  Clock,
  ScrollBar,
  Draw
};

ModeList PanelMode = Clock;


void PanelCallback(int event, int type) 
{ 
  // PanelCallback is called when a message is received from the panel 
  // only channel and type are passed as parameters. Panel data available as public library vars

   switch (PanelMode)
   {
     case Clock: 
       ClockCallback( event, type); break;
     
     case AllButton:
       AllButtonCallback( event, type); break;

     case ScrollBar: 
       ScrollBarCallback( event, type); break;

     case Draw: 
       DrawCallback( event, type); break;

     default: 
       break;
   }
}






