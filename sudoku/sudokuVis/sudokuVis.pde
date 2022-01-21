import javafx.util.*;
float heightf;
float widthf;
int n;
int sqrtn;
final int m = 2;
float[][][][] data;
float[][][] confidence;
int[][][] boardVal;
int[][] constraintBoard;
int num_iterations;
float min_val = 999999;
float max_val = -999999;
colourLegend cl;
int cur_it = 0;
String filename = "inputs/extension.txt";
boolean confidenceMode = false;
boolean displayViolations = true;
boolean displayMatchSoln = true;
boolean recording = false;

int cur_movie = 2;
color borderColour;
boolean checkPermutation(int[] perm){
  boolean[] seen = new boolean[n];
  for (int i = 0; i < n; i++){
    seen[i] = false;
    
  }
  for (int i = 0; i < n; i++){
    if (seen[perm[i]-1]){
      return false;
    }
    seen[perm[i]-1] = true;
  }
  return true;
}

void setup() {
  
  colorMode(HSB,255);
  borderColour = color(0,0,50);
  fullScreen();
  heightf = height;
  widthf = width;

  BufferedReader reader = createReader(filename);
  String line = null;

  try {
    line = reader.readLine();
    
    // Assuming n is a square, and also hoping for no float shennanigans 
    n = int(line.split(" ")[0]);
    //checkPermutation(new int[] {1,2,3,4,5,6,7,8,9});
    sqrtn = round(sqrt(n));
    float inputFactor = float(line.split(" ")[1]);

    data = new float[10005][n][n][n];
    constraintBoard = new int[n][n];
    boardVal = new int[10005][n][n];
    confidence = new float[10005][n][n];
    for (int y = 0; y < n; y++){
      line = reader.readLine();
      String[] pieces = split(line, " ");
      for (int x = 0; x < n; x++){
        constraintBoard[x][y] = int(pieces[x]);
      }
    }
    int k = 0;
    boolean flag = true;
    while (flag) {
      for (int y = 0; y < n && flag; y++) {
        for (int x = 0; x < n && flag; x++){
          line = reader.readLine();
          String[] pieces = split(line, " ");
          if (pieces.length == 1) {
            flag = false;
            num_iterations = k;
            break;
          }

          for (int i = 0; i < n; i++){  
             data[k][x][y][i] = float(pieces[i])/inputFactor;
             min_val = min(min_val, data[k][x][y][i]);
             max_val = max(max_val,data[k][x][y][i]);
          }
          
          // Very ugly code to calculate the largest and second largest, because Java is ugly.
          int m1 = -9999;
          int m2 = -9999;
          int i1 = -1;
          int i2 = -1;
          for (int i = 0; i < n; i++){
            int v = int(pieces[i]);
            if (v >= m1){
              i2 = i1;
              i1 = i;
              m2 = m1;
              m1 = v;
            }
            else if (v >= m2){
              i2 = i;
              m2 = v;
            }
          }
          confidence[k][x][y] = (float)(m1-m2) / (float)inputFactor;
          boardVal[k][x][y] = i1 + 1;
        }        
      }
      k++;
    }
    reader.close();
  } 
  catch (IOException e) {
    e.printStackTrace();
  }
  cl = new colourLegend(n * 60 + 100, n * 30, 30, n * 60, min_val,max_val, "");
  println(min_val);
  println(max_val);
}

void draw() {
  clear();
  translate(dmx, dmy);
  scale(scaling); 

  //ellipse(width/2, height/2, 100, 100);
  if (confidenceMode){
    for (int y = 0; y < n; y++){
      for (int x = 0; x < n; x++){
        float val = confidence[cur_it][x][y];
        if (displayMatchSoln && boardVal[cur_it][x][y] == boardVal[num_iterations-1][x][y]){
          fill(map_val_to_colour3(val,min_val,max_val));
        }
        else{
          fill(map_val_to_colour2(val,min_val,max_val));
        }
        stroke(borderColour);
        strokeWeight(2);
        rect(x*60,y*60,60,60);
        textAlign(CENTER, CENTER);
        // white
        fill(0,0,255,255);
        textSize(50);
        text(str(boardVal[cur_it][x][y]),x*60 + 30, y * 60 + 25);
        if (constraintBoard[x][y] != -1){
          textAlign(LEFT, TOP);
          textSize(10);
          text(str(constraintBoard[x][y]+1),x*60 + 5, y * 60 + 2);
        }
      }
    }
  }
  else{
    for (int y = 0; y < n; y++){
      for (int x = 0; x < n; x++){
        
        for (int i = 0; i < n; i++){
          
          float val = data[cur_it][x][y][i];
          fill(map_val_to_colour(val,min_val,max_val));
          stroke(borderColour);
          strokeWeight(1);
          int bx = i % sqrtn;
          int by = sqrtn - 1 - i / sqrtn;
          rect(x * 60 + bx * 20, y * 60 + by * 20, 20,20);
  
          // ew american spelling
          textAlign(CENTER, CENTER);
          // grey
          fill(0,0,125);
          textSize(10);
          text(str(i+1),x * 60 + bx * 20+10, y * 60 + by * 20+10);
        }      
        
        
      }
    }
    for (int y = 0; y < n; y++){
      for (int x = 0; x < n; x++){
        // Draw a sorta thicc black border for normal cells
        fill(0,0,0,0);
        stroke(borderColour);
        strokeWeight(3);
        rect(x * 60, y * 60, 60, 60);
        if (constraintBoard[x][y] != -1){
          textAlign(CENTER, CENTER);        
          // white
          fill(0,0,255,125);
          textSize(50);
          text(str(constraintBoard[x][y]+1),x*60 + 30, y * 60 + 25);
        }
      }
    }
  }
  for (int i = 0; i <= sqrtn; i++){
    strokeWeight(6);
    stroke(borderColour);
    line(sqrtn * i * 60, 0, sqrtn * i * 60, n * 60);
    line(0, sqrtn * i * 60, n * 60, sqrtn * i * 60);
  }
  
  if (displayViolations){
    for (int y = 0; y < n; y++){
      for (int x = 0; x < n; x++){
        if (constraintBoard[x][y] != -1 && constraintBoard[x][y] + 1 != boardVal[cur_it][x][y]){
          strokeWeight(2);
          stroke(0,255,255);
          fill(0,0,0,0);
          int k = 3;
          rect(x * 60 + k, y * 60 + k, 60 - 2 * k, 60 - 2 * k);
        }
      }
    }
    for (int i = 0; i < n; i ++){
      int[] hPerm = new int[9];
      int[] vPerm = new int[9];
      int[] bPerm = new int[9];
      
      for (int j = 0; j < n; j++){
        hPerm[j] = boardVal[cur_it][j][i];
        vPerm[j] = boardVal[cur_it][i][j];
        int bx = i%3;
        int by = i/3;
        int x = bx * 3 + j%3;
        int y = by * 3 + j/3;
        bPerm[j] = boardVal[cur_it][x][y];
      }
      if (!checkPermutation(hPerm)){
        stroke(0,255,255);
        strokeWeight(3);
        line(-30,i*60 + 30, 0, i*60+30);        
      }
      if (!checkPermutation(vPerm)){
        stroke(0,255,255);
        strokeWeight(3);
        line(i*60 + 30, -30, i*60+30,0);        
      }
      if (!checkPermutation(bPerm)){
        int x = i%3;
        int y = i/3;
        stroke(0,255,255);
        strokeWeight(2);
        noFill();
        //noStroke();
        //fill(0,255,255,50);
        int k = 3;
        rect(x*60 * sqrtn + k ,y*60 * sqrtn + k,60 * sqrtn - 2 * k,60 * sqrtn - 2 * k);
      }
    }
  }
  
  
  cl.render_bar();
  cl.render_labels();
  textSize(40);
  textAlign(LEFT);
  text("Current tick: " + str(cur_it),0, n * 60 + 60);

  if (recording){
    if (cur_it == 0 || cur_it == num_iterations-1){
      for (int i = 0; i < 10; i++){
        saveFrame("recordings/outputs" + str(cur_movie) + "/####.png");
      }
    }
    else{
      saveFrame("recordings/outputs" + str(cur_movie) + "/####.png");
    }
    if (cur_it == num_iterations-1){
      recording = false;
    }
    else{
      cur_it++;
    }
  }
}
