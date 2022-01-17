float heightf;
float widthf;
int n;
final int m = 2;
float[][][] data;
int num_iterations;
float min_val = 999999;
float max_val = -999999;
colourLegend cl;
int cur_it = 0;
String filename = "20fail.out";
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
    n = int(line);

    data = new float[5005][n][n];
    int k = 0;
    boolean flag = true;
    while (flag) {
      for (int y = 0; y < n; y++) {
        line = reader.readLine();
        String[] pieces = split(line, " ");
        if (pieces.length == 1) {
          flag = false;
          num_iterations = k;
          break;
        }
        for (int x=  0; x < n; x++){  
           data[k][x][y] = float(pieces[x]);
           min_val = min(min_val, data[k][x][y]);
           max_val = max(max_val,data[k][x][y]);
        }
      }
      k++;
    }
    reader.close();
  } 
  catch (IOException e) {
    e.printStackTrace();
  }
  cl = new colourLegend(n * 50 + 100, n * 25, 30, n * 50, min_val,max_val, "");
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
      float val =data[cur_it][x][y];
      if (roundMode){
        val = round(val);
      }
      
      fill(map_val_to_colour(val,min_val,max_val));
      rect(x*50,y*50,50,50);
    }
  }
  cl.render_bar();
  cl.render_labels();
  textSize(40);
  textAlign(LEFT);
  text("Current tick: " + str(cur_it),0, n * 50 + 60);
  
  for (int i = 0; i < n; i++){
    int hcnt = 0;
    int vcnt = 0;
    for (int j = 0; j < n; j++){
        hcnt += round(data[cur_it][j][i]);
        vcnt += round(data[cur_it][i][j]);
    }
    
    stroke(0,255,255);
    strokeWeight(3);
    if (hcnt != m){
      fill(0,255,255);
      line(0, i * 50 + 25, -20, i * 50 + 25);
    }
    if (vcnt != m){
      fill(0,255,255);
      line(i * 50 + 25, 0, i * 50 + 25, -20);
    }
    int[] dcnt = new int[4];
    for (int j = 0; j <= i; j++){
      dcnt[0] += round(data[cur_it][i-j][j]);
      dcnt[1] += round(data[cur_it][n-1-(i-j)][n-1-j]);
      dcnt[2] += round(data[cur_it][n-1-(i-j)][j]);
      dcnt[3] += round(data[cur_it][i-j][n-1-j]);
    }
    
    if (dcnt[0] > m){
      line(0, (i+1) * 50, -20, (i+1)*50 + 20);
    }
    if (dcnt[1] > m){
      line((n-i-1) * 50, n * 50, (n-i-1) * 50 - 20, n * 50 + 20);
    }
    if (dcnt[2] > m){
      line(0, (n-i-1) * 50, -20, (n-i-1)* 50 - 20);
    }
    if (dcnt[3] > m){
      line((n-i-1) * 50, 0, (n-i-1) * 50 - 20, -20);
    }
    stroke(0,0,0);
    strokeWeight(2);
  }
  if (recording){
    if (cur_it == 0 || cur_it == num_iterations-1){
      for (int i = 0; i < 10; i++){
        saveFrame("outputs" + str(cur_movie) + "/####.png");
      }
    }
    else{
      saveFrame("outputs" + str(cur_movie) + "/####.png");
    }
    if (cur_it == num_iterations-1){
      recording = false;
    }
    else{
      cur_it++;
    }
  }
}
