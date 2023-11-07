#include "llvm/IR/PassManager.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Analysis/LoopInfo.h"
#include "llvm/IR/Instructions.h"
#include <string>
#include <stdio.h>
#include <iostream>
#include <numeric>
#include <stdlib.h> 
#include <cmath>
using namespace llvm;
using namespace std;
namespace {

void diophantine(int x1,int x2,int y1,int y2,int lower,int upper,string name1,string name2,int index1,int index2,string type){
    int a=x1,b=0-y1;
    int g=gcd(abs(a),abs(b));
    int c=y2-x2,i=1,u,v;
    upper=upper-1;
    while(1){
      if(((g-(a*i))%b)==0){
        u=i;
        v=(g-(a*i))/b;
        if(u==0 || v==0){
          i++;
          continue;
        }
        break;
      }else{
        i++;
      }
    }
    
    int x=u*c/g,xt=b/g;
    int y=v*c/g,yt=0-(a/g);
    //cout << u << " " << v << " " << c << " " << g << endl;
    //cout << x << " " << xt << " " << y << " " << yt << endl;
    float l1=lower-x,r1=upper-x,tep,fl,fr;
    float l2=lower-y,r2=upper-y;
    if(xt<0){
      tep=l1;
      l1=r1/xt;
      r1=tep/xt;
      tep=l2;
      l2=r2/yt;
      r2=tep/yt;
    }else{
      l1=l1/xt;
      r1=r1/yt;
      l2=l2/xt;
      r2=r2/yt;
    };

    //cout << l1 << " " << r1 << " " << l2 << " " << r2 << endl;
    if(r1<r2){
      fr=floor(r1);
    }else{
      fr=floor(r2);
    };

    if(l1>l2){
      fl=ceil(l1);
    }else{
      fl=ceil(l2);
    };
    //cout << fl << " " << fr << endl;
    int i1,i2;
    for(int i=fl;i<=fr;i++){
      i1=(x+xt*i);
      i2=(y+yt*i);
      if(type=="flow"){
        if(i1<=i2){
          if(i1==i2){
            if(index1<index2){
              cout << "(i="<<i1<< ", i="<<i2 <<")"<< endl;
              cout << name1 << ": S"<< index1+1 << " -----> S" << index2+1 <<endl;
            }
          }else{
            cout << "(i="<<i1<< ", i="<<i2 <<")"<< endl;
            cout << name1 << ": S"<< index1+1 << " -----> S" << index2+1 <<endl;
          }
        }
      }else if(type=="anti"){
        if(i1<=i2){
          if(i1==i2){
            if(index1<index2){
              cout << "(i="<<i1<< ", i="<<i2 <<")"<< endl;
              cout << name1 << ": S"<< index1+1 << " --A--> S" << index2+1 <<endl;
            }
          }else{
            cout << "(i="<<i1<< ", i="<<i2 <<")"<< endl;
            cout << name1 << ": S"<< index1+1 << " --A--> S" << index2+1 <<endl;
          }
        }
      }else if(type=="output"){
        if(i1<=i2){
          if(i1==i2){
            if(index1<index2){
              cout << "(i="<<i1<< ", i="<<i2 <<")"<< endl;
              cout << name1 << ": S"<< index1+1 << " --O--> S" << index2+1 <<endl;
            }
          }else{
            cout << "(i="<<i1<< ", i="<<i2 <<")"<< endl;
            cout << name1 << ": S"<< index1+1 << " --O--> S" << index2+1 <<endl;
          }
        }
      }
    }

}

struct HW1Pass : public PassInfoMixin<HW1Pass> {
  PreservedAnalyses run(Function &F, FunctionAnalysisManager &FAM);
};

PreservedAnalyses HW1Pass::run(Function &F, FunctionAnalysisManager &FAM) {
  //errs() << "[HW1]: " << F.getName() << '\n';
  int ifinValue;
  int istValue;
  string sdname[5],ssname[5];
  int sdindex[5]={0},ssindex[5]={0};
  int sdmindex[5]={1,1,1,1,1},ssmindex[5]={1,1,1,1,1};
  Value* ibounds;
  for (BasicBlock &BB : F) {
  
      if(BB.getName()=="for.cond"){
        for (Instruction &I : BB) {
          //get i finish value
          if (auto *sltI = dyn_cast<CmpInst>(&I)){
            if (ConstantInt* CI = dyn_cast<ConstantInt>(I.getOperand(1))) {
                if (CI->getBitWidth() <= 32) {
                  ifinValue = CI->getSExtValue();
                  //errs() << "I finish values: " << ifinValue << '\n';
                }
              }
          }
        }
      }
      
      if(BB.getName()=="entry"){
        for (Instruction &I : BB){
          //get i start value
          if (auto *swI = dyn_cast<StoreInst>(&I)){
              if(I.getOperand(1)->getName() == "i"){
                if (ConstantInt* CI = dyn_cast<ConstantInt>(I.getOperand(0))) {
                  if (CI->getBitWidth() <= 32) {
                    istValue = CI->getSExtValue();
                    //errs() << "I start value: " << istValue << '\n';
                  }
                }
              }
            
          }
          
          if(auto *LI = dyn_cast<LoadInst>(&I)){
            
          }
      }
    }
      int sindex=0,sddiff=1,sddiff2=1;
      if(BB.getName()=="for.body"){
          sindex=0;
          sddiff=1;
          sddiff2=1;
          for(Instruction &I : BB){
              if(sddiff==1){
                  if (auto *GI = dyn_cast<GetElementPtrInst>(&I)){
                      ssname[sindex]=string(I.getOperand(0)->getName());
                      if(sddiff==2){
                        sddiff=1;
                      }else{
                        sddiff++;
                      }          
                  }else if (auto *BI = dyn_cast<BinaryOperator>(&I)){
                      if (!strncmp(I.getOpcodeName(), "sub", 4)){
                            //errs() << I << '\n';
                          if (ConstantInt* CI = dyn_cast<ConstantInt>(I.getOperand(1))) {
                              if (CI->getBitWidth() <= 32) {
                                ssindex[sindex] = 0-(CI->getSExtValue());
                                //cout << "sub: " << ssindex[sindex] << "sindex: " << sindex << endl;
                              }
                            }
                          }else if(!strncmp(I.getOpcodeName(), "add", 4)){
                            if (ConstantInt* CI = dyn_cast<ConstantInt>(I.getOperand(1))) {
                              if (CI->getBitWidth() <= 32) {
                                ssindex[sindex] = CI->getSExtValue();
                                cout << "add: " <<ssindex[sindex] << endl;
                              }
                            }
                          }else if(!strncmp(I.getOpcodeName(), "mul", 4)){
                            if (ConstantInt* CI = dyn_cast<ConstantInt>(I.getOperand(0))) {
                              if (CI->getBitWidth() <= 32) {
                                ssmindex[sindex] = CI->getSExtValue();
                                //cout << "mul: " <<ssmindex[sindex] << endl;
                              }
                            }
                          }
                          
                    }else if (auto *SI = dyn_cast<StoreInst>(&I)){
                        sindex++;
                    }
            }else if(sddiff==2){
                if (auto *GI = dyn_cast<GetElementPtrInst>(&I)){
                    sdname[sindex]=string(I.getOperand(0)->getName());
                    if(sddiff==2){
                      sddiff=1;
                    }else{
                      sddiff++;
                    }          
                }else if (auto *BI = dyn_cast<BinaryOperator>(&I)){
                  
                    if (!strncmp(I.getOpcodeName(), "sub", 4)){
                      //errs() << I << '\n';
                      if (ConstantInt* CI = dyn_cast<ConstantInt>(I.getOperand(1))) {
                        if (CI->getBitWidth() <= 32) {
                          sdindex[sindex] = 0-(CI->getSExtValue());
                          //cout << "sub: " << sdindex[sindex] << "sindex: " << sindex << endl;
                        }
                      }
                    }else if(!strncmp(I.getOpcodeName(), "add", 4)){
                      if (ConstantInt* CI = dyn_cast<ConstantInt>(I.getOperand(1))) {
                        if (CI->getBitWidth() <= 32) {
                          sdindex[sindex] = CI->getSExtValue();
                          //cout << "add: " <<sdindex[sindex] << endl;
                        }
                      }
                    }else if(!strncmp(I.getOpcodeName(), "mul", 4)){
                      if (ConstantInt* CI = dyn_cast<ConstantInt>(I.getOperand(0))) {
                        if (CI->getBitWidth() <= 32) {
                          sdmindex[sindex] = CI->getSExtValue();
                          //cout << "mul: " <<sdmindex[sindex] << endl;
                        }
                      }
                    }
                }else if (auto *SI = dyn_cast<StoreInst>(&I)){
                    sindex++;
                }
            }
          

        }
        //cout << sdname[0] << " " << ssname[0] << endl;
        //cout << sdname[1] << " " << ssname[1] << endl;
        //cout << sdname[2] << " " << ssname[2] << endl;
        cout << "====Flow Dependency====" << endl;
        for(int i=0;i<sindex;i++){
          for(int j=0;j<sindex;j++){
            if(i!=j){
              if(sdname[i]==ssname[j]){
                diophantine(sdmindex[i],sdindex[i],ssmindex[j],ssindex[j],istValue,ifinValue,sdname[i],ssname[i],i,j,"flow");
              }
            }
            
          }
        }
        cout << "====Anti-Dependency====" << endl;
        for(int i=0;i<sindex;i++){
          for(int j=0;j<sindex;j++){
            if(i!=j){
              if(ssname[i]==sdname[j]){
                diophantine(ssmindex[i],ssindex[i],sdmindex[j],sdindex[j],istValue,ifinValue,ssname[i],sdname[i],i,j,"anti");
              }
            }
            
          }
        }
        cout << "====Output Dependency====" << endl;
        for(int i=0;i<sindex;i++){
          for(int j=0;j<sindex;j++){
            if(i!=j){
              if(sdname[i]==sdname[j]){
                diophantine(sdmindex[i],sdindex[i],sdmindex[j],sdindex[j],istValue,ifinValue,sdname[i],sdname[i],i,j,"output");
              }
            }
          }
        }

    }
   
  }
    auto &LI = FAM.getResult<LoopAnalysis>(F);
    for (const auto &L : LI) {
        
    }
  return PreservedAnalyses::all();
}

} // end anonymous namespace

extern "C" ::llvm::PassPluginLibraryInfo LLVM_ATTRIBUTE_WEAK
llvmGetPassPluginInfo() {
  return {LLVM_PLUGIN_API_VERSION, "HW1Pass", LLVM_VERSION_STRING,
          [](PassBuilder &PB) {
            PB.registerPipelineParsingCallback(
                [](StringRef Name, FunctionPassManager &FPM,
                   ArrayRef<PassBuilder::PipelineElement>) {
                  if (Name == "hw1") {
                    FPM.addPass(HW1Pass());
                    return true;
                  }
                  return false;
                });
          }};
}