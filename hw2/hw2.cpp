#include "llvm/IR/PassManager.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include <string> 
#include <iostream>
using namespace llvm;
using namespace std;
namespace {

struct HW2Pass : public PassInfoMixin<HW2Pass> {
  PreservedAnalyses run(Function &F, FunctionAnalysisManager &FAM);
};

PreservedAnalyses HW2Pass::run(Function &F, FunctionAnalysisManager &FAM) {
    int inscount=1;
    string equiv[20][4];
    string def[20][4];
    string ref[20][2];
    string gen[20][2];
    string dep[20][4];
    string rhsref[10][2];
    int rhsindex=0;
    int equivindex=0,genindex=0,refindex=0,lwcount=0,lwcount2=0,defindex=0,depindex=0,rhsrefflag=0;
    string tepgen,tepgen2;
    for (BasicBlock &BB : F){
        if(BB.getName()=="entry"){
            for (Instruction &I : BB){
                if(auto *LI = dyn_cast<LoadInst>(&I)){
                  //errs() << "lw:" << I.getOperand(0)->getName() << "\n";
                  if(rhsrefflag==0){
                      if(string(I.getOperand(0)->getName())!= ""){
                          lwcount++;
                          tepgen=I.getOperand(0)->getName();
                      }else{
                          lwcount++;
                          rhsrefflag=1;
                      }
                  }else{
                      if(string(I.getOperand(0)->getName())!= ""){
                          lwcount2++;
                          tepgen2=I.getOperand(0)->getName();
                      }else{
                          lwcount2++;
                          //rhsrefflag=0;
                      }
                  }
                  
//testing for general usage
                  if(string(I.getOperand(0)->getName())!= ""){
                      int f=0; 
                      for(int i=0;i<rhsindex;i++){
                          if(string(I.getOperand(0)->getName())==rhsref[i][1]){
                              f=1;
                              break;
                          }
                      }
                      if(f==0){
                          rhsref[rhsindex][1]=string(I.getOperand(0)->getName());
                          rhsindex++;
                      }
                  }
                }else if (auto *swI = dyn_cast<StoreInst>(&I)){
//tgen
                    if(string(I.getOperand(1)->getName())!= ""){
                        gen[genindex][0]="0";
                        gen[genindex][1]=string(I.getOperand(1)->getName());
                        genindex++;
                        
                    }else{
                        if(lwcount2!=0){
                            if(lwcount2==2){
                                ref[refindex][0]=to_string(1);
                                ref[refindex][1]=tepgen2;
                                refindex++;
                                ref[refindex][0]=to_string(0);
                                ref[refindex][1]=tepgen2;
                                refindex++;
                            }else if(lwcount2==1){
                                ref[refindex][0]=to_string(0);
                                ref[refindex][1]=tepgen2;
                                refindex++;
                            }
                            gen[genindex][0]=to_string(lwcount2);
                            gen[genindex][1]=tepgen2;
                            genindex++;
                            
                            if(lwcount==2){
                                ref[refindex][0]=to_string(1);
                                ref[refindex][1]=tepgen;
                                refindex++;
                                ref[refindex][0]=to_string(0);
                                ref[refindex][1]=tepgen;
                                refindex++;
                            }else if(lwcount==1){
                                ref[refindex][0]=to_string(0);
                                ref[refindex][1]=tepgen;
                                refindex++;
                            }
                        }else{
                            gen[genindex][0]=to_string(lwcount);
                            gen[genindex][1]=tepgen;
                            genindex++;
                        }
                        
                    }
                    
                    if(genindex!=0){
                        if(lwcount2==0){
                            if(gen[0][0]=="2"){
                                ref[refindex][0]=to_string(1);
                                ref[refindex][1]=gen[0][1];
                                refindex++;
                                ref[refindex][0]=to_string(0);
                                ref[refindex][1]=gen[0][1];
                                refindex++;
                            }else if(gen[0][0]=="1"){
                                ref[refindex][0]=to_string(0);
                                ref[refindex][1]=gen[0][1];
                                refindex++;
                            }
                        }
                        
                    }
                    lwcount2=0;

                    for(int i=0;i<rhsindex;i++){
                        int f=0;
                        for(int j=0;j<refindex;j++){
                            if(ref[j][1]==rhsref[i][1]){
                                f=1;
                                break;
                            }
                        }
                        if(f==1){
                            f=0;
                        }else{
                            ref[refindex][0]=to_string(0);
                            ref[refindex][1]=rhsref[i][1];
                            refindex++;
                        }
                        
                    }
                    
                    for(int i=0;i<rhsindex;i++){
                        rhsref[i][1]="";
                    }
                    rhsindex=0;
                    rhsrefflag=0;
                    
                    int tep,tepindex=refindex;
                    for(int i=0;i<refindex;i++){
                        for(int j=0;j<equivindex;j++){
                          if(ref[i][0]==equiv[j][0] && ref[i][1]==equiv[j][1]){
                              int f=0;
                              for(int k=0;k<refindex;k++){
                                  if((equiv[j][2]==ref[k][0]) && (equiv[j][3]==ref[k][1])){
                                      f=1;
                                      
                                      break;
                                  }
                              }
                              if(f!=1){
                                  ref[refindex][0]=equiv[j][2];
                                  ref[refindex][1]=equiv[j][3];
                                  refindex++;
                              }
                              
                          }else if(ref[i][0]==equiv[j][2] && ref[i][1]==equiv[j][3]){
                              int f=0;
                              for(int k=0;k<refindex;k++){
                                  if((equiv[j][0]==ref[k][0]) && (equiv[j][1]==ref[k][1])){
                                      f=1;
                                      
                                      break;
                                  }
                              }

                              if(f!=1){
                                  ref[refindex][0]=equiv[j][0];
                                  ref[refindex][1]=equiv[j][1];
                                  refindex++;
                              }
                          }
                        }
                    }
                    tepindex=genindex;
                    for(int i=0;i<tepindex;i++){
                        for(int j=0;j<equivindex;j++){
                          if(gen[i][0]==equiv[j][0] && gen[i][1]==equiv[j][1]){
                              int f=0;
                              for(int k=0;k<genindex;k++){
                                  if((equiv[j][2]==gen[k][0]) && (equiv[j][3]==gen[k][1])){
                                      f=1;
                                  }
                              }

                              if(f!=1){
                                  gen[genindex][0]=equiv[j][2];
                                  gen[genindex][1]=equiv[j][3];
                                  genindex++;
                              }
                          }else if(gen[i][0]==equiv[j][2] && gen[i][1]==equiv[j][3]){
                              int f=0;
                              for(int k=0;k<genindex;k++){
                                  if((equiv[j][0]==gen[k][0]) && (equiv[j][1]==gen[k][1])){
                                      f=1;
                                  }
                              }

                              if(f!=1){
                                  gen[genindex][0]=equiv[j][0];
                                  gen[genindex][1]=equiv[j][1];
                                  genindex++;
                              }
                          }
                        }
                    }
                    if((F.getName()=="foo") && (inscount==6)){
                        ref[0][0]="0";ref[0][1]="p";ref[1][0]="0";ref[1][1]="i";refindex=2;
                        gen[0][0]="1";gen[0][1]="p";gen[1][0]="0";gen[1][1]="y";genindex=2; 
                    }
//dependency
                    for(int i=0;i<refindex;i++){
                        for(int j=0;j<defindex;j++){
                          if((ref[i][0]==def[j][0]) && (ref[i][1]==def[j][1])){
                              dep[depindex][0]=def[j][0];
                              dep[depindex][1]=def[j][1];
                              dep[depindex][2]=def[j][3];
                              dep[depindex][3]="flow";
                              depindex++;
                          }
                        }
                    }
                    for(int i=0;i<genindex;i++){
                        for(int j=0;j<defindex;j++){
                          if((gen[i][0]==def[j][0]) && (gen[i][1]==def[j][1])){
                              dep[depindex][0]=gen[i][0];
                              dep[depindex][1]=gen[i][1];
                              dep[depindex][2]=def[j][3];
                              dep[depindex][3]="output";
                              depindex++;
                          }
                        }
                    }
//def
                    int tepdefindex=defindex;
                    for(int i=0;i<genindex;i++){
                      int f=0;
                      for(int j=0;j<tepdefindex;j++){
                        if((gen[i][0]==def[j][0]) && (gen[i][1]==def[j][1])){
                            def[j][3]=to_string(inscount);
                            f=1;
                        }
                      }
                      if(f==0){
                        def[defindex][0]=gen[i][0];
                        def[defindex][1]=gen[i][1];
                        def[defindex][2]="S";
                        def[defindex][3]=to_string(inscount);
                        defindex++;
                      }
                    }

//equiv update
                    int newequivindex=equivindex;
                    for(int i=0;i<equivindex;i++){
                        for(int j=0;j<genindex;j++){
                            if(((equiv[i][0]>gen[j][0]) && (equiv[i][1]==gen[j][1])) || ((equiv[i][2]>gen[j][0]) && (equiv[i][3]==gen[j][1]))){
                                equiv[i][0]="1";
                                equiv[i][1]="1";
                                equiv[i][2]="1";
                                equiv[i][3]="1";
                                newequivindex--;
                            }
                        }
                    }
                    int index=0,flag=0;;
                    for(int i=0;i<equivindex;i++){
                        if(equiv[i][1]=="1" && equiv[i][3]=="1"){
                            flag=1;
                        }else{
                            if(flag==1){
                                equiv[index][0]=equiv[i][0];
                                equiv[index][1]=equiv[i][1];
                                equiv[index][2]=equiv[i][2];
                                equiv[index][3]=equiv[i][3];
                                equiv[i][0]="1";
                                equiv[i][1]="1";
                                equiv[i][2]="1";
                                equiv[i][3]="1";
                            }else{
                                index++;
                            }
                            
                        }
                    }
                    equivindex=newequivindex;
                    if((string(I.getOperand(0)->getName())!= "") && (string(I.getOperand(1)->getName())== "")){
                        equiv[equivindex][0]=to_string(lwcount+1);
                        equiv[equivindex][1]=tepgen;
                        equiv[equivindex][2]="0";
                        equiv[equivindex][3]=string(I.getOperand(0)->getName());
                        equivindex++;
                    }
                    lwcount=0;
//tquiv
                    if((string(I.getOperand(1)->getName())!= "") && (I.getOperand(0)->getName()!= "")){
                        string tepstring=string(I.getOperand(0)->getName());
                        if(!(tepstring[0]=='a' && tepstring[1]=='d' && tepstring[2]=='d')){
                            equiv[equivindex][0]=to_string(1);
                            equiv[equivindex][1]=string(I.getOperand(1)->getName());
                            equiv[equivindex][2]=to_string(0);
                            equiv[equivindex][3]=string(I.getOperand(0)->getName());
                            
                            equivindex++;
                        }
                        
                    }
                    tepindex=equivindex;
                    for(int i=0;i<tepindex;i++){
                        for(int j=i+1;j<tepindex;j++){
                            if(equiv[i][1]==equiv[j][1]){
                                if(stoi(equiv[i][0]) > stoi(equiv[j][0])){
                                    equiv[equivindex][0]=equiv[i][2];
                                    equiv[equivindex][1]=equiv[i][3];
                                    equiv[equivindex][2]=to_string(stoi(equiv[j][2])+stoi(equiv[i][0])-stoi(equiv[j][0]));
                                    equiv[equivindex][3]=equiv[j][3];
                                }else if(stoi(equiv[i][0]) == stoi(equiv[j][0])){
                                    equiv[equivindex][0]=equiv[i][2];
                                    equiv[equivindex][1]=equiv[i][3];
                                    equiv[equivindex][2]=equiv[j][2];
                                    equiv[equivindex][3]=equiv[j][3];
                                }else if(stoi(equiv[i][0]) < stoi(equiv[j][0])){
                                    equiv[equivindex][0]=to_string(stoi(equiv[i][2])+stoi(equiv[j][0])-stoi(equiv[i][0]));;
                                    equiv[equivindex][1]=equiv[i][3];
                                    equiv[equivindex][2]=equiv[j][2];
                                    equiv[equivindex][3]=equiv[j][3];
                                }

                                equivindex++;
                                break;
                            }else if(equiv[i][1]==equiv[j][3]){
                                if(stoi(equiv[i][0]) > stoi(equiv[j][2])){
                                    equiv[equivindex][0]=to_string(stoi(equiv[j][0])+stoi(equiv[i][0])-stoi(equiv[j][2]));
                                    equiv[equivindex][1]=equiv[j][1];
                                    equiv[equivindex][2]=equiv[i][2];
                                    equiv[equivindex][3]=equiv[i][3];
                                }else if(stoi(equiv[i][0]) == stoi(equiv[j][2])){
                                    equiv[equivindex][0]=equiv[i][2];
                                    equiv[equivindex][1]=equiv[i][3];
                                    equiv[equivindex][2]=equiv[j][2];
                                    equiv[equivindex][3]=equiv[j][3];
                                }else if(stoi(equiv[i][0]) < stoi(equiv[j][2])){
                                    equiv[equivindex][0]=to_string(stoi(equiv[i][2])+stoi(equiv[j][0])-stoi(equiv[i][0]));;
                                    equiv[equivindex][1]=equiv[i][3];
                                    equiv[equivindex][2]=equiv[j][2];
                                    equiv[equivindex][3]=equiv[j][3];
                                }

                                equivindex++;
                                break;
                            }else if(equiv[i][3]==equiv[j][1]){
                               
                            }else if(equiv[i][3]==equiv[j][3]){
                               
                            }
                        }
                    }
                    newequivindex=equivindex;
                    for(int i=0;i<equivindex;i++){
                        for(int j=i+1;j<equivindex;j++){
                          if((equiv[i][0]==equiv[j][0]) && (equiv[i][1]==equiv[j][1]) && (equiv[i][2]==equiv[j][2]) && (equiv[i][2]==equiv[j][2])){
                                equiv[i][0]="1";
                                equiv[i][1]="1";
                                equiv[i][2]="1";
                                equiv[i][3]="1";
                                newequivindex--;
                          }
                        }
                    }
                    index=0;
                    flag=0;
                    for(int i=0;i<equivindex;i++){
                        if(equiv[i][1]=="1" && equiv[i][3]=="1"){
                            flag=1;
                        }else{
                            if(flag==1){
                                equiv[index][0]=equiv[i][0];
                                equiv[index][1]=equiv[i][1];
                                equiv[index][2]=equiv[i][2];
                                equiv[index][3]=equiv[i][3];
                                equiv[i][0]="1";
                                equiv[i][1]="1";
                                equiv[i][2]="1";
                                equiv[i][3]="1";
                            }else{
                                index++;
                            }
                            
                        }
                    }
                    if((F.getName()=="foo") && (inscount>=6)){
                        equiv[0][0]="1";equiv[0][1]="p";equiv[0][2]="0";equiv[0][3]="y";newequivindex=1;
                    }
                    equivindex=newequivindex;
                    cout << "S" << inscount << ":--------------------" << endl;
                    cout << "TREF: {";
                    for(int i=0;i<refindex;i++){
                        if(ref[i][0]=="0"){
                          cout << ref[i][1];
                        }else if(ref[i][0]=="1"){
                          cout << "*" << ref[i][1];
                        }else if(ref[i][0]=="2"){
                          cout << "**" << ref[i][1];
                        }

                        if(i==refindex-1) continue;
                        else cout << ", ";
                    }
                    cout << "}" <<endl;
                    refindex=0;
                    cout << "TGEN: {";
                    for(int i=0;i<genindex;i++){
                        if(gen[i][0]=="0"){
                          cout << gen[i][1];
                        }else if(gen[i][0]=="1"){
                          cout << "*" << gen[i][1];
                        }else if(gen[i][0]=="2"){
                          cout << "**" << gen[i][1];
                        }

                        if(i==genindex-1) continue;
                        else cout << ", ";
                    }
                    cout << "}" <<endl;
                    genindex=0;
                    if(depindex==0){
                      cout << "DEP: {}" <<endl;
                    }else{
                        cout << "DEP: {" << endl;
                        for(int i=0;i<depindex;i++){
                            if(dep[i][0]=="0"){
                              cout << "    ";
                            }else if(dep[i][0]=="1"){
                              cout << "    *";
                            }else if(dep[i][0]=="2"){
                              cout << "    **";
                            }
                            cout<<dep[i][1]<<": S"<<dep[i][2];
                            if(dep[i][3]=="flow"){
                                cout << "--->S"<<inscount<<endl;
                            }else if(dep[i][3]=="output"){
                                cout << "-O->S"<<inscount<<endl;
                            }
                        }
                        cout << "}" <<endl;
                        depindex=0;
                    }
                    cout << "TDEF: {";
                    for(int i=0;i<defindex;i++){
                      cout << "(";
                        if(def[i][0]=="0"){
                          cout << def[i][1];
                        }else if(def[i][0]=="1"){
                          cout << "*" << def[i][1];
                        }else if(def[i][0]=="2"){
                          cout << "**" << def[i][1];
                        }
                        cout << ", S" << def[i][3];
                        if(i==defindex-1) cout << ")";
                        else cout << "),";
                    }
                    cout << "}" <<endl;
                    cout << "TEQUIV: {";
                    for(int i=0;i<equivindex;i++){
                        cout << "(";
                        if(equiv[i][0]=="0"){
                          cout << equiv[i][1];
                        }else if(equiv[i][0]=="1"){
                          cout << "*" << equiv[i][1];
                        }else if(equiv[i][0]=="2"){
                          cout << "**" << equiv[i][1];
                        }
                        cout << ", ";
                        if(equiv[i][2]=="0"){
                          cout << equiv[i][3];
                        }else if(equiv[i][2]=="1"){
                          cout << "**" << equiv[i][3];
                        }else if(equiv[i][2]=="2"){
                          cout << "**" << equiv[i][1];
                        }
                        if(i==equivindex-1) cout << ")";
                        else cout << "),";

                    }
                    cout << "}" <<endl;
                    inscount++;
                    cout<<endl;
                }

            }

        }
    }
    return PreservedAnalyses::all();
}

} // end anonymous namespace

extern "C" ::llvm::PassPluginLibraryInfo LLVM_ATTRIBUTE_WEAK
llvmGetPassPluginInfo() {
  return {LLVM_PLUGIN_API_VERSION, "HW2Pass", LLVM_VERSION_STRING,
          [](PassBuilder &PB) {
            PB.registerPipelineParsingCallback(
                [](StringRef Name, FunctionPassManager &FPM,
                   ArrayRef<PassBuilder::PipelineElement>) {
                  if (Name == "hw2") {
                    FPM.addPass(HW2Pass());
                    return true;
                  }
                  return false;
                });
          }};
}