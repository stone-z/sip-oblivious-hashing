#pragma once

#include "DependencyAnaliser.h"
#include "definitions.h"

#include <memory>

namespace llvm {
class GlobalVariable;
class LoopInfo;
}

namespace input_dependency {

class VirtualCallSiteAnalysisResult;

class FunctionAnaliser
{
public:
    FunctionAnaliser(llvm::Function* F,
                     llvm::AAResults& AAR,
                     llvm::LoopInfo& LI,
                     const VirtualCallSiteAnalysisResult& virtualCallsInfo,
                     const FunctionAnalysisGetter& getter);

public:
    void setFunction(llvm::Function* F);

public:
    /**
     * \brief Preliminary analyses input dependency of instructions in the function.
     * Results of this function is primary information about instructions input dependency, output arguments input dependency, and function call sites dependency info.
     * \note This information is not complete as it is based on assumption, that function input arguments are also program inputs.
     *       - Information about function calls from this function, whith additional information about called function arguments being input dependent or not.
     *
     * \note The call site information can be obtained with \link getCallSitesData function.
     * \note To make analysis results final call \link finalizeArguments and \link finalizeGlobals after calling this function.
     */
    void analize();

    /**
     * \brief Finalizes input dependency analysis by refining \link analize results with given set of input dependent arguments.
     * \param[in] inputDepArgs Arguments which are actually input dependent.
     * 
     * \note \link analize function should be called before calling this function.
     */
    void finalizeArguments(const DependencyAnaliser::ArgumentDependenciesMap& inputDepArgs);


    /**
     * \brief Finalizes input dependency analysis by refining \link analize results with given globals dependency info.
     * \param[in] globalsDeps globals dependency information collected from other functions.
     * 
     * \note Only instructions which depend on any global from \a globalsDeps may be affected by this call.
     * \note \link analize function should be called before calling this function.
     */
    void finalizeGlobals(const DependencyAnaliser::GlobalVariableDependencyMap& globalsDeps);

    /// Returns set of functions which have call sites in \a m_F
    FunctionSet getCallSitesData() const;

    /**
    * \brief Get arguments dependency information for the given function, collected by this analiser
    * \param F Function for which dependency info should be returned.
    * \note \a F should have call site in \a m_F
    */
    const DependencyAnaliser::ArgumentDependenciesMap& getCallArgumentInfo(llvm::Function* F) const;
    // can't return with reference. try get with r-reference to avoid copy
    FunctionCallDepInfo getFunctionCallDepInfo(llvm::Function* F) const;

    /**
    * \brief Get globals dependency information for the given function, collected by this analiser
    * \param F Function for which dependency info should be returned.
    * \note \a F should have call site in \a m_F
    */
    DependencyAnaliser::GlobalVariableDependencyMap getCallGlobalsInfo(llvm::Function* F) const;

    /**
     * \brief Checks if instruction is input dependent.
     * \param[in] instr Instruction to check
     *
     * \note If called after after \a analize but before \a finalize, the result may be incomplete.
     *
     * \see \link analize
     * \see \link finalize
     * \see \link isInputDependent
     */
    bool isInputDependent(llvm::Instruction* instr) const;
    bool isInputDependent(const llvm::Instruction* instr) const;
    bool isInputIndependent(llvm::Instruction* instr) const;
    bool isInputIndependent(const llvm::Instruction* instr) const;

    /// Checks if \a m_F makes its output parameter \a arg dependent to input.
    bool isOutArgInputIndependent(llvm::Argument* arg) const;

    /**
    * \brief Returns input dependency information for the given \a arg.
    *        If there is no dependency information for \a arg, returns invalid dep info.
    */
    DepInfo getOutArgDependencies(llvm::Argument* arg) const;
    bool isReturnValueInputIndependent() const;
    const DepInfo& getRetValueDependencies() const;
    bool hasGlobalVariableDepInfo(llvm::GlobalVariable* global) const;
    const DepInfo& getGlobalVariableDependencies(llvm::GlobalVariable* global) const;

    const GlobalsSet& getReferencedGlobals() const;
    const GlobalsSet& getModifiedGlobals() const;

    llvm::Function* getFunction();
    const llvm::Function* getFunction() const;

    void dump() const;

private:
    class Impl;
    std::shared_ptr<Impl> m_analiser;
};

} // namespace input_dependency

