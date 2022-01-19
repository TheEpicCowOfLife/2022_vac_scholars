float heightf;
float widthf;
int n;
int sqrtn;
final int m = 2;
float[][][][] data;
int[][] constraintBoard;
int num_iterations;
float min_val = 999999;
float max_val = -999999;
colourLegend cl;
int cur_it = 0;
String filename = "inputs/test.txt";
boolean roundMode = false;
boolean recording = false;
int cur_movie = 0;

void setup() {
  fullScreen();
  heightf = height;
  widthf = width;

  BufferedReader reader = createReader(filename);
  String line = null;

  try {
    line = reader.readLine();
    
    // Assuming n is a square, and also hoping for no float shennanigans 
    n = int(line.split(" ")[0]);
    sqrtn = round(sqrt(n));
    float inputFactor = float(line.split(" ")[1]);

    data = new float[10005][n][n][n];
    constraintBoard = new int[n][n];
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
  for (int y = 0; y < n; y++){
    for (int x = 0; x < n; x++){
      
      for (int i = 0; i < n; i++){
        
        float val = data[cur_it][x][y][i];
        fill(map_val_to_colour(val,min_val,max_val));
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
      stroke(0);
      strokeWeight(4);
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
