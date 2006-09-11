#!/usr/bin/python

import string, sys
sys.path.append('../src/')
import H5Part

def ReadFile(fn):
    print "Open " , fn
  
    file = H5Part.H5PartOpenFile(fn, H5Part.H5PART_READ) 
  
    nt = H5Part.H5PartGetNumSteps(file) 
    H5Part.H5PartSetStep(file,0) 
    nds = H5Part.H5PartGetNumDatasets(file)   

    print "Timesteps = " , nt , " dataSets per timestep = " , nds 
  
    print "\n===============================\n" 
    
    for i in range(0, nds): 
        value, name = H5Part.H5PartGetDatasetName(file, i, 64) 
        #print name
        print "\tDataset[%u] name=[%s]\n" % (i, name) 
        
    print "\n===============================\n" 
  
    for steps in range (0, nt):
        H5Part.H5PartSetStep(file,steps) 
        n = H5Part.H5PartGetNumParticles(file) 
        print "number of particles in %d step =" % steps , n , "\n" 
        x=H5Part.doubleArray(n) 
        y=H5Part.doubleArray(n) 
        z=H5Part.doubleArray(n) 
        px=H5Part.doubleArray(n) 
        py=H5Part.doubleArray(n) 
        pz=H5Part.doubleArray(n) 
        id=H5Part.longArray(n) 
    
    H5Part.H5PartReadParticleStep(file,steps,x,y,z,px,py,pz,id) 
    
    sumx = 0.0 
    sumpz = 0.0 
    
    for i in range (0,n):    
        sumx += x[i] 
        sumpz += pz[i] 
    
    print "\t================================================="
    print "\tstep= " , steps , " sum(x)= " , sumx , " sum(pz)= " , sumpz 
    print "\tfirst x is " , x[0] , "\tlast x is " , x[n-1]
    print "\tFor fake data, expect sumx to be =" , x[0]*n 
    print "\t=================================================\n"

    H5Part.H5PartCloseFile(file) 
    return 1 

def WriteFile(fn):
    nt = 5
    np = 1024*1024 
    print "Open " , fn 
  
    file= H5Part.H5PartOpenFile(fn,H5Part.H5PART_WRITE) 
  
    H5Part.H5PartWriteFileAttribString(file,"File Description", "This file is created by H5test.py. Simple H5Part file for testing purpose...")
    #fileA = 2000
    #H5Part.H5PartWriteFileAttrib(file,"File Number will be 2000", H5Part.H5T_NATIVE_INT64, fileA, 1) #201326626 H5T_NATIVE_INT64
    
    x=H5Part.doubleArray(np) 
    y=H5Part.doubleArray(np) 
    z=H5Part.doubleArray(np) 
    px=H5Part.doubleArray(np) 
    py=H5Part.doubleArray(np) 
    pz=H5Part.doubleArray(np) 
    id=H5Part.longArray(np) 

    H5Part.H5PartSetNumParticles(file,np)
  
    for n in range(0,np):
        id[n]=n 
        x[n]=1.0 
        y[n]=2.0 
        z[n]=3.0 
        px[n]=1.0*n*(n%10)
        py[n]=2.0*n*(n%10) 
        pz[n]=3.0*n*(n%10) 
      
    for t in range(0,nt):
        H5Part.H5PartSetStep(file,t) 
        print "Write Step %u\n" % t 
        H5Part.H5PartWriteDataFloat64(file,"x",x) 
        H5Part.H5PartWriteDataFloat64(file,"y",y) 
        H5Part.H5PartWriteDataFloat64(file,"z",z) 
        H5Part.H5PartWriteDataFloat64(file,"px",px) 
        H5Part.H5PartWriteDataFloat64(file,"py",py) 
        H5Part.H5PartWriteDataFloat64(file,"pz",pz) 
        H5Part.H5PartWriteDataInt64(file,"id",id) 
  
    H5Part.H5PartCloseFile(file) 
    return 1 


if __name__ == "__main__":
    fn ="H5test_py.h5" 

    if WriteFile(fn) == 0:
        print "Failed to write file " , fn
        sys.exit(0)

    if ReadFile(fn) == 0:
        print "Failed to read file " , fn
        sys.exit(0)
