#ifndef _PIXELISER_H_
#define _PIXELISER_H_

#include <vector>
#include <iostream>
#include <fstream>

#include "TCanvas.h"
#include "TH2.h"
#include "TStyle.h"


//----------------------------------------------------------------------
// class pixeliser

template <typename T>
class pixeliser{
public:

  pixeliser(int channel, int height, int width);
  ~pixeliser();

  void fill(int ich, int ix, int iy, T val);
  void fill(int ich, float R, float fx, float fy, T val);

  // for plot
  void print();
  void set_title(std::string title);
  void set_maximum(float zmax);
  void plot(std::string out_dir, std::string prefix);

  // save matrices
  void save(std::string out_dir, std::string prefix);

private:
  int _channel;
  int _height;
  int _width;

  std::vector<std::vector<T>> _mat;

  int _index(int ix, int iy);
  int _get_index(float R, float dr, int width);

  // for plot
  float _zmax=200;
  std::vector<std::vector<float>> _xv;
  std::vector<std::vector<float>> _yv;
  std::vector<std::vector<T>> _zv;
  std::vector<std::string> _title;

};

//----------------------------------------------------------------------
// constructor

template <typename T>
pixeliser<T>::pixeliser(int channel, int height, int width){
  if(!(std::is_same<T, int>() || std::is_same<T, float>() || std::is_same<T, double>())){
    std::cout << "Class Pixeliser: Unsupported value type!" << std::endl;
    exit(1);
  }

  _channel = channel;
  _width = width;
  _height = height;

  int _nval = _width * _height;

  _mat.clear();
  _xv.clear();
  _yv.clear();
  _zv.clear();
  for(int i=0; i<_channel; i++){
    _mat.emplace_back(std::vector<T>(_nval, 0));
    _xv.emplace_back(std::vector<float>());
    _yv.emplace_back(std::vector<float>());
    _zv.emplace_back(std::vector<T>());
  }

}

// destructor

template <typename T>
pixeliser<T>::~pixeliser(){}

//----------------------------------------------------------------------
// fill a pixel

template <typename T>
void pixeliser<T>::fill(int ich, int ix, int iy, T val){
  int idx = _index(ix, iy);

  // skip when out of range
  if(idx == -1) return;

  _mat[ich][idx] += val;
  return;
}

template <typename T>
void pixeliser<T>::fill(int ich, float R, float fx, float fy, T val){
  _xv[ich].push_back(fx);
  _yv[ich].push_back(fy);
  _zv[ich].push_back(val);

  int ix = _get_index(R, fx, _width);
  int iy = _get_index(R, fy, _height);

  // in case of overflow
  if(ix<0 || ix>_width || iy<0 || iy>_height) return;
  fill(ich, ix, iy, val);

  return;
}

template <typename T>
int pixeliser<T>::_get_index(float R, float dr, int width){
  int ix=0;
  if(width%2 == 0){
    if(dr>=0) ix = int(0.5*dr*width/R) + width/2;
    else ix = int(0.5*dr*width/R) + width/2 - 1;
  }
  else{
    if(dr>=0) ix = int(0.5*dr*width/R + 0.5) + width/2;
    else ix = int(0.5*dr*width/R - 0.5) + width/2;
  }
  return ix;
}


//----------------------------------------------------------------------
// get pixel index

template <typename T>
int pixeliser<T>::_index(int ix, int iy){
  if(ix>=_width || iy >= _height){
    std::cout << "Class Pixeliser: index out of range" << std::endl;
    return -1;
  }
  return (_height-iy-1)*_width + ix;
}

//----------------------------------------------------------------------
// print matrix
template <typename T>
void pixeliser<T>::print(){
  for(auto&& m : _mat){
    for(int i=0; i<m.size(); i++){
      std::cout << m[i];
      if((i+1)%_width == 0){
        std::cout << std::endl;
        continue;
      }
      std::cout << ",";
    }
    std::cout << std::endl;
  }
  return;
}

//----------------------------------------------------------------------
// set title
template <typename T>
void pixeliser<T>::set_title(std::string title){
  std::string s = title;
  std::string delimiter = ";";

  size_t pos = 0;
  std::string token;
  while ((pos = s.find(delimiter)) != std::string::npos) {
    token = s.substr(0, pos);
    _title.push_back(token);
    s.erase(0, pos + delimiter.length());
  }
  _title.push_back(s);

  if(_title.size() != _channel){
    std::cout << "pixeliser::set_title(): wrong title!" << std::endl;
  }
  return;
}

template <typename T>
void pixeliser<T>::set_maximum(float zmax){
  _zmax = zmax;
  return;
}

//----------------------------------------------------------------------
// plot image
template <typename T>
void pixeliser<T>::plot(std::string out_dir, std::string prefix){
  std::string ofn = out_dir + prefix + ".pdf";

  std::vector<TH2F*> hv;
  for(int ich=0; ich<_channel; ich++){
    std::string hn = std::to_string(ich);
    TH2F* _h = new TH2F(hn.c_str(), "", _width, -0.4, 0.4, _height, -0.4, 0.4);
    if(!_title.empty())
      _h->SetTitle(Form("%s;%s;%s", _title[ich].c_str(), "#Delta#eta", "#Delta#phi"));
    else
      _h->SetTitle(Form("%s;%s;%s", "", "#Delta#eta", "#Delta#phi"));
    _h->SetMinimum(0);
    _h->SetMaximum(_zmax);

    for(int i=0; i<_zv[ich].size(); i++){
      _h->Fill((float) _xv[ich][i], (float) _yv[ich][i], (float) _zv[ich][i]);
    }
    hv.push_back(_h);
  }

  int nv = (int) std::sqrt(_channel);
  int nh = (int) (_channel/nv);


  int npx = nv * 400;
  int npy = nh * 400;

  TCanvas* c = new TCanvas("c", "c", npx, npy);
  c->Divide(nv, nh);

  for(int ich=0; ich<_channel; ich++){
    c->cd(ich+1);
    gPad->SetMargin(0.1, 0.15, 0.1, 0.1);
    gStyle->SetOptStat(0);
    hv[ich]->Draw("colz");
  }
  c->SaveAs(ofn.c_str());

  // free memory
  for(auto&& p : hv){
    delete p;
  }
  delete c;
  return;
}


//----------------------------------------------------------------------
// save csv file
template <typename T>
void pixeliser<T>::save(std::string out_dir, std::string prefix){
  std::string ofn = out_dir + prefix + ".csv";
  std::ofstream ofs(ofn, std::ofstream::out);

  for(auto&& m : _mat){
    for(int i=0; i<m.size(); i++){
      ofs << m[i];
      if((i+1)%_width == 0){
        ofs << std::endl;
        continue;
      }
      ofs << ",";
    }
    ofs << std::endl;
  }
  ofs.close();
  return;
}


#endif // _PIXELISER_H_
