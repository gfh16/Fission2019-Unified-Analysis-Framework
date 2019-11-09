// 对着TSpectru的源代码,手动输入一遍,来了解源代码

void Search(float *spectrum, int size, double sigma)
{
  int xmin,xmax,i,j,l,i1,i2,i3,i5,ni,n2,n3,stav,peak_index,lmin,lmax;
  i1 = i2 = i3 =0;
  double a,b,s,f,si4,fi4,suma,sumai,sold,fold,norm,filter[PEAK_WINDOW];
  si4 = fi4 =0;
  for(i=0; i<PEAK_WINDOW; i++) filter[i]=0;
  j = (int)


}
