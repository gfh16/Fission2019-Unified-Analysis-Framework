//______________________________________________________________________________
void Search1(float *spectrum,int size,double sigma)
{
/////////////////////////////////////////////////////////////////////////////
//   ONE-DIMENSIONAL PEAK SEARCH FUNCTION                                  //
//   This function searches for peaks in source spectrum                   //
//   The number of found peaks and their positions are written into        //
//   the members fNpeaks and fPositionX.                                   //
//                                                                         //
//   Function parameters:                                                  //
//   source:  pointer to the vector of source spectrum                     //
//   size:    length of source spectrum                                    //
//   sigma:   sigma of searched peaks, for details we refer to manual      //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////
   int xmin,xmax,i,j,l,i1,i2,i3,i5,n1,n2,n3,stav,peak_index,lmin,lmax;
   i1 = i2 = i3 = 0;
   double a,b,s,f,si4,fi4,suma,sumai,sold,fold=0,norma,filter[PEAK_WINDOW];
   si4 = fi4 = 0;
   for(i=0;i<PEAK_WINDOW;i++) filter[i]=0;
   j=(int)(3.0*sigma);
   for(i=-j;i<=j;i++) {
      a = i;
      a = -a*a;
      b = 2.0*sigma*sigma;
      a = a/b;
      a = exp(a);
      s = i;
      s = s*s;
      s = s-sigma*sigma;
      s = s/(sigma*sigma*sigma*sigma);
      s = s*a;
      filter[PEAK_WINDOW/2+i]=s;
   }
   norma = 0;
   for(i=0;i<PEAK_WINDOW;i++) norma=norma+TMath::Abs(filter[i]);
   for(i=0;i<PEAK_WINDOW;i++) filter[i]=filter[i]/norma;
   suma  = 0;
   sumai = 0;
   stav  =1 ;
   peak_index = 0;
   sold = PEAK_WINDOW/2;
   xmin = (int)(3.0*sigma);
   xmax = size-(int)(3.0*sigma);
   lmin = PEAK_WINDOW/2-(int)(3.0*sigma);
   lmax = PEAK_WINDOW/2+(int)(3.0*sigma);
   for(i=xmin;i<=xmax;i++) {
       s = 0;
       f = 0;
      for(l=lmin;l<=lmax;l++) {
         if(i+l-PEAK_WINDOW/2 >= size) break;
         a  = spectrum[i+l-PEAK_WINDOW/2];
         s += a*filter[l];
         f += a*filter[l]*filter[l];
      }
      f = sqrt(f);
      if (s<0) {
         a      = i;
         a     *= s;
         suma  += s;
         sumai += a;
      }
      if ((stav==1)&&(s>f)) {
stav1:
         stav = 2;
         suma = 0;
         sumai= 0;
         i1   = i;
      }
      else if ((stav==2)&&(s<=f)) {
         stav = 3;
         i2   = i;
      }
      else if(stav==3) {
         if (s>f)
            goto stav1;
         if (s<=0) {
            stav = 4;
            i3   = i;
         }
      }
      else if((stav==4)&&(s>=sold)) {
         si4 = sold;
         fi4 = fold;
         stav= 5;
      }
      else if((stav==5)&&(s>=0)) {
         stav = 6;
         i5   = i;
         if (si4==0)
            stav = 0;
         else{
            n1 = i5-i3+1;
            a  = n1+2;
            a  = fi4*a/(2.*si4)+1/2.;
            a  = TMath::Abs(a);
            n2 = (int)a;
            a  = n1-4; if (a<0) a=0;
            a  = a*(1-2.*(fi4/si4))+1/2.;
            a  = TMath::Abs(a);
            n3 = (int)(a/fResolution);
            a  = TMath::Abs(si4); if(a<=(2.*fi4)) stav=0;
            if (n2>=1) {
               if ((i3-i2-1)>n2)
                  stav = 0;
            }
            else {
               if ((i3-i2-1)>1) stav = 0;
            }
            if ((i2-i1+1)<n3) stav=0;
         }
         if (stav!=0) {
            b = sumai/suma;
            if (peak_index < fMaxPeaks) {
               fPositionX[peak_index] = b;
               peak_index += 1;
            } else {
               Warning("Search1","PEAK BUFFER FULL");
               return 0;
            }
         }
         stav  = 1;
         suma  = 0;
         sumai = 0;
      }
      sold = s;
      fold = f;
   }
   fNPeaks = peak_index;
   return fNPeaks;
}
