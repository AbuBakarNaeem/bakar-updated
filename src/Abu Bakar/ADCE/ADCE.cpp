
#define DEBUG_TYPE "hello"
#include "llvm/ADT/Statistic.h"
#include "llvm/ADT/DepthFirstIterator.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/IntrinsicInst.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/Pass.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/InstIterator.h"
#include "llvm/Support/CFG.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"
#include "llvm/Transforms/Utils/BasicBlockUtils.h"
#include "llvm/Transforms/Utils/UnifyFunctionExitNodes.h"
#include "llvm/Transforms/Utils/Local.h"
#include "llvm/Transforms/Scalar.h"
#include "llvm/Analysis/AliasAnalysis.h"
#include "llvm/Analysis/PostDominators.h"
#include "llvm/Support/CommandLine.h"
#include <iostream>
#include <stdio.h>      
#include <stdlib.h>     
#include <time.h>       
#include <algorithm>
using namespace llvm;
using namespace std;

namespace {
  struct ADCE : public FunctionPass {
    static char ID; // Pass identification, replacement for typeid
    ADCE() : FunctionPass(ID) {}


    void markLive(Instruction* I, set<Instruction*>&  LiveSet, vector<Instruction*>&  WorkList) {
      if(LiveSet.find(I) == LiveSet.end()) {
        LiveSet.insert(I);
        WorkList.push_back(I);
      }
    }
    void markDead(Instruction* I, vector<Instruction*>&  Dead) {
      if(find(Dead.begin(), Dead.end(), I) == Dead.end()) {
        Dead.push_back(I);
      }
    }
    bool isTriviallyLive(Instruction* I) {
      return I->mayHaveSideEffects() || isa<ReturnInst>(I) || isa<TerminatorInst>(I);
    }
    void removeDeadBBs(Function &F) {
      vector<BasicBlock*> DeadBBs;
      for (Function::iterator f_it = F.begin(), f_ite = F.end(); f_it != f_ite; ++f_it) {
        BasicBlock* BB = f_it;
        if(BB->getTerminator () == NULL) {
          DeadBBs.push_back(BB);
        }
      }
      for(int i = 0; i < DeadBBs.size(); i++) {
        DeadBBs[i]->dropAllReferences();
      }
      for(int i = 0; i < DeadBBs.size(); i++) {
        DeadBBs[i]->eraseFromParent();
      }
    }

    bool runOnFunction(Function &F) {
      set<Instruction*> alive;
      vector<Instruction*>  Dead;
      vector<Instruction*> worklist;
      int count = 0;
      int removed = 0;
      SmallPtrSet<BasicBlock*, 8> Reachable;
      BasicBlock* first = F.begin();
      for (df_ext_iterator<BasicBlock*, SmallPtrSet<BasicBlock*, 8> > Iter =
        df_ext_begin(first, Reachable), End = df_ext_end(first, Reachable); Iter != End; ++Iter) {
        BasicBlock* BB = *Iter;
        for(BasicBlock::iterator b_it = BB->begin(), b_ite = BB->end(); b_it != b_ite; ++b_it) {
          Instruction* I = b_it;
          count++;
          if(isTriviallyLive(I)) {
            markLive(I, alive, worklist);
          }
          else if(cast<Value>(I)->use_empty()) {
            if(find(Dead.begin(), Dead.end(), I) == Dead.end()) {
              I->print(errs());
              errs() << "\n";
              Dead.push_back(I);
            }
          	removed++;
          }
        }
      }
      for(int i = 0; i < Dead.size(); i++) {
        Dead[i]->eraseFromParent();
      }
      Dead.clear();
      while(!worklist.empty()) {
        Instruction* curr = worklist[0];
        worklist.erase(worklist.begin());
        User* user = cast<User>(curr);
        int num = user->getNumOperands();
        for(int i = 0; i < num; i++) {
          Value* op = user->getOperand(i);
          if(isa<Instruction>(op)) {
            markLive(cast<Instruction>(op), alive, worklist);
          }
        }
      }
      for (Function::iterator f_it = F.begin(), f_ite = F.end(); f_it != f_ite; ++f_it) {
        for(BasicBlock::iterator b_it = f_it->begin(), b_ite = f_it->end(); b_it != b_ite; ++b_it) {
          Instruction* I = b_it;
          if(alive.find(I) == alive.end() && find(Dead.begin(), Dead.end(), I) == Dead.end()) {
            I->print(errs());
            errs() << "\n";
            Dead.push_back(I);
          }
        }
      }
      for(int i = 0; i < Dead.size(); i++) {
        Dead[i]->dropAllReferences();
      }
      for(int i = 0; i < Dead.size(); i++) {
        Dead[i]->eraseFromParent();
      }
      removed += Dead.size();
      errs() << "Total Instructions : " << count << "\n";
      errs() << "Number Removed : " << removed << "\n";
      removeDeadBBs(F);
      return true;
    }
    // We don't modify the program, so we preserve all analyses.
    virtual void getAnalysisUsage(AnalysisUsage &AU) const {
      AU.addRequired<UnifyFunctionExitNodes>(); // Unify returns from function
      AU.addRequired<PostDominatorTree>(); // Need postdominator info
      AU.setPreservesCFG(); // CFG is not modified in ADCE
    }
  };
}

char ADCE::ID = 0;
static RegisterPass<ADCE>
Z("ADCE", "ADCE implementation");

namespace {
  struct PrintAll : public FunctionPass {
    static char ID;
    PrintAll() : FunctionPass(ID) {}

    bool runOnFunction(Function &F) {
      errs() << "\n \n \n";
      int count = 1;
      for (Function::iterator f_it = F.begin(), f_ite = F.end(); f_it != f_ite; ++f_it) {
        for(BasicBlock::iterator b_it = f_it->begin(), b_ite = f_it->end(); b_it != b_ite; ++b_it) {
          Instruction* Ins = b_it;
          errs() << count << " ";
          Ins->print(errs());
          errs() << "\n";
          count++;
        }
      }
      return false;
    }
  };
}

char PrintAll::ID = 0;
static RegisterPass<PrintAll>
W("PrintAll", "Hello World Pass (with getAnalysisUsage implemented)");