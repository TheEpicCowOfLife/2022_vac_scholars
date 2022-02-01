float dmx = 0;
float dmy = 0;
float scaling = 1;

void mouseDragged() {
  dmx += mouseX-pmouseX;
  dmy += mouseY-pmouseY;
}
void mouseWheel(MouseEvent event) {
  scaling += scaling * event.getCount()*-0.05;
  dmx = -(mouseX - dmx)*(1+event.getCount()*-0.05) + mouseX;
  dmy = -(mouseY - dmy)*(1+event.getCount()*-0.05) + mouseY;
}

void keyPressed(){
  
    if (key == 'r'){
      recording ^= true;
    }
    else if (key == 'w'){
      cur_it = min(num_iterations-1, cur_it+1);
    }
    else if (key == 'W'){
      cur_it = min(num_iterations-1, cur_it+100);
    }
    else if(key == 'q'){
      cur_it = max(0, cur_it - 1);
    }
    else if(key == 'Q'){
      cur_it = max(0, cur_it - 100);
    }
    else if (key == 'e'){
      roundMode ^= true; 
    }

}
