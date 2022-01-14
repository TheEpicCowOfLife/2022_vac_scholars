// shamelessly stolen from 2018 Quang.
// this function is what powers the linear colour gradient

// takes in a value val with values between lb and ub (lower bound and upper bound),
// and then maps a colour to it on a linear scale between lb and ub. Any point outside the
// range simply gets clamped to either lb or ub.

// by the way is this how you're meant to have default parameters in java???
public color map_val_to_colour(float val, float lb, float ub){
  return map_val_to_colour(val,lb,ub,255);
}

public color map_val_to_colour(float val, float lb, float ub, float alpha){
  colorMode(HSB, 255);
  
  return color(
    map(val, lb, ub, 180, 0), // hue
    map(val, lb, ub, 100, 255), // saturation
    map(val, lb, ub, 100, 255), // brightness
    alpha
  );
}
