// shamelessly stolen from 2018 Quang.
class colourLegend{
  PImage img; // the bar itself is just stored as an image. This is so that it has minimal performance impact.
  String title;
  float x; // position of centre
  float y;
  int xd; // dimensions of the bar
  int yd;
  
  float max_val; //maximum value of the colour scale
  float min_val;
  
  colourLegend(float xp, float yp, int xd, int yd, float min_val, float max_val, String title){
    this.title = title;
    this.x = xp;
    this.y = yp;
    this.xd = xd;
    this.yd = yd;
    
    this.max_val = max_val;
    this.min_val = min_val;
    
    // initialises the pixels of img
    this.img = createImage(xd,yd,RGB);
    this.img.loadPixels();
    for (int y = 0; y < yd; y++){
      for (int x = 0; x < xd; x++){
        this.img.pixels[y*xd + x] = map_val_to_colour(yd-y,0,yd);
      }
    }
    this.img.updatePixels();
    
  }
  
  // renders the bar itself.
  void render_bar(){
    image(this.img,this.x-(float)this.xd/2, this.y-(float)this.yd/2);
  }
  
  int num_labels = 4; // number of increments
  // this is a lie, there's num_labels + 1 increments, to fit a label for 0.
  void render_labels(){
    fill(255,255,255);
    // render the title of the legend
    textSize(20 * heightf/1080.0);
    textAlign(CENTER);
    text(this.title,this.x,this.y - this.yd/2 - 80 * heightf/1080);
    
    // render the labels on the side.
    textAlign(RIGHT, CENTER); 
    for (int i = num_labels; i >= 0; i--){
      // casting hell :(
      // val is the value to be displayed, and the value that the triangle points to.
      float val = map(i,0,num_labels,this.min_val,this.max_val);
      
      // y is the y coordinate of the point to be labelled.
      float y = this.y + this.yd/2 - (float)i * (float)this.yd/(float)this.num_labels;
      
      fill(map_val_to_colour(val,this.min_val,this.max_val));
      noStroke();
      // render a triangle pointing there
      // this function is literally "specify 3 points". Well here goes.
      triangle(
        this.x - this.xd/2, // vertex 1: the one pointing to the bar
        y,
        
        this.x - this.xd/2 - 20*heightf/1080.0, // vertex 2, the upper left one.
        y - 10 * heightf/1080.0,
        
        this.x - this.xd/2 - 20*heightf/1080.0, // vertex 3, the bottom left one.
        y + 10 * heightf/1080.0
      );
      
      // draws the text itself.
      fill(0,0,255);     
      text(String.format("%.2f",val), this.x - this.xd/2 - 30*heightf/1080.0,  y);
    }
  }
  
  
}
