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
  // WARNING: r is pretty dangerous to press, it'll lag everything out and keep recording. Only make sure
  // you want to record, and have correctly set the recording output.
  if (key == 'r'){
    recording ^= true;
  }
  // Press w to increase iterations one at a time
  else if (key == 'w'){
    cur_it = min(num_iterations-1, cur_it+1);
  }
  // Hold shift while pressing W to skip 100 iterations at a time.
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
    confidenceMode ^= true; 
  }
  else if (key == 'a'){
    displayViolations ^= true;
  }
  else if (key == 's'){
    displayMatchSoln ^= true;
  }
  else if (key == 'd'){
    drawBoth ^= true;
  }

}
