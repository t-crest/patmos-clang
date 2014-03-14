//===--- Tools.h - Tool Implementations -------------------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef CLANG_LIB_DRIVER_TOOLS_H_
#define CLANG_LIB_DRIVER_TOOLS_H_

#include "clang/Driver/Tool.h"
#include "clang/Driver/Types.h"
#include "clang/Driver/Util.h"
#include "llvm/ADT/Triple.h"
#include "llvm/Option/Option.h"
#include "llvm/Support/Compiler.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/Path.h"

namespace clang {
  class ObjCRuntime;

namespace driver {
  class Command;
  class Driver;

namespace toolchains {
  class Darwin;
}

namespace tools {
using llvm::opt::ArgStringList;

  /// \brief Clang compiler tool.
  class LLVM_LIBRARY_VISIBILITY Clang : public Tool {
  public:
    static const char *getBaseInputName(const llvm::opt::ArgList &Args,
                                        const InputInfoList &Inputs);
    static const char *getBaseInputStem(const llvm::opt::ArgList &Args,
                                        const InputInfoList &Inputs);
    static const char *getDependencyFileName(const llvm::opt::ArgList &Args,
                                             const InputInfoList &Inputs);

  private:
    void AddPreprocessingOptions(Compilation &C, const JobAction &JA,
                                 const Driver &D,
                                 const llvm::opt::ArgList &Args,
                                 llvm::opt::ArgStringList &CmdArgs,
                                 const InputInfo &Output,
                                 const InputInfoList &Inputs) const;

    void AddPatmosTargetArgs(const llvm::opt::ArgList &Args,
	                     llvm::opt::ArgStringList &CmdArgs) const;
    void AddAArch64TargetArgs(const llvm::opt::ArgList &Args,
                              llvm::opt::ArgStringList &CmdArgs) const;
    void AddARMTargetArgs(const llvm::opt::ArgList &Args,
                          llvm::opt::ArgStringList &CmdArgs,
                          bool KernelOrKext) const;
    void AddMIPSTargetArgs(const llvm::opt::ArgList &Args,
                           llvm::opt::ArgStringList &CmdArgs) const;
    void AddR600TargetArgs(const llvm::opt::ArgList &Args,
                           llvm::opt::ArgStringList &CmdArgs) const;
    void AddSparcTargetArgs(const llvm::opt::ArgList &Args,
                            llvm::opt::ArgStringList &CmdArgs) const;
    void AddSystemZTargetArgs(const llvm::opt::ArgList &Args,
                              llvm::opt::ArgStringList &CmdArgs) const;
    void AddX86TargetArgs(const llvm::opt::ArgList &Args,
                          llvm::opt::ArgStringList &CmdArgs) const;
    void AddHexagonTargetArgs(const llvm::opt::ArgList &Args,
                              llvm::opt::ArgStringList &CmdArgs) const;

    enum RewriteKind { RK_None, RK_Fragile, RK_NonFragile };

    ObjCRuntime AddObjCRuntimeArgs(const llvm::opt::ArgList &args,
                                   llvm::opt::ArgStringList &cmdArgs,
                                   RewriteKind rewrite) const;

    void AddClangCLArgs(const llvm::opt::ArgList &Args,
                        llvm::opt::ArgStringList &CmdArgs) const;

  public:
    Clang(const ToolChain &TC) : Tool("clang", "clang frontend", TC) {}

    virtual bool hasGoodDiagnostics() const { return true; }
    virtual bool hasIntegratedAssembler() const { return true; }
    virtual bool hasIntegratedCPP() const { return true; }

    virtual void ConstructJob(Compilation &C, const JobAction &JA,
                              const InputInfo &Output,
                              const InputInfoList &Inputs,
                              const llvm::opt::ArgList &TCArgs,
                              const char *LinkingOutput) const;
  };

  /// \brief Clang integrated assembler tool.
  class LLVM_LIBRARY_VISIBILITY ClangAs : public Tool {
  public:
    ClangAs(const ToolChain &TC) : Tool("clang::as",
                                        "clang integrated assembler", TC) {}

    virtual bool hasGoodDiagnostics() const { return true; }
    virtual bool hasIntegratedAssembler() const { return false; }
    virtual bool hasIntegratedCPP() const { return false; }

    virtual void ConstructJob(Compilation &C, const JobAction &JA,
                              const InputInfo &Output,
                              const InputInfoList &Inputs,
                              const llvm::opt::ArgList &TCArgs,
                              const char *LinkingOutput) const;
  };

  /// patmos specific compilation flow.
namespace patmos {

  class PatmosBaseTool {
    const ToolChain &TC;
    std::vector<std::string> LibraryPaths;
  public:
    PatmosBaseTool(const ToolChain &TC) : TC(TC) {}

  protected:
    // Some helper methods to construct arguments in ConstructJob
    llvm::sys::fs::file_magic getFileType(std::string filename) const;

    llvm::sys::fs::file_magic getBufFileType(const char *buf) const;

    bool isBitcodeFile(std::string filename) const {
      return getFileType(filename) == llvm::sys::fs::file_magic::bitcode;
    }

    bool isArchive(std::string filename) const {
      return getFileType(filename) == llvm::sys::fs::file_magic::archive;
    }

    bool isDynamicLibrary(std::string filename) const;

    bool isBitcodeArchive(std::string filename) const;

    bool isBitcodeOption(StringRef Option,
                         const std::vector<std::string> &LibPaths) const;

    const char * CreateOutputFilename(Compilation &C, const InputInfo &Output,
                                      const char * TmpPrefix,
                                      const char *Suffix,
                                      bool IsLastPass) const;

    /// Get the option value of an argument
    std::string getArgOption(const std::string &Arg) const;

    std::string FindLib(StringRef LibName,
                        const std::vector<std::string> &Directories,
                        bool OnlyStatic) const;

    std::vector<std::string> FindBitcodeLibPaths(const llvm::opt::ArgList &Args,
                                                 bool LookupSysPaths) const;

    /// Get the last -O<Lvl> optimization level specifier. If no -O option is
    /// given, return NULL.
    llvm::opt::Arg* GetOptLevel(const llvm::opt::ArgList &Args, char &Lvl) const;

    /// Add -L arguments
    void AddLibraryPaths(const llvm::opt::ArgList &Args, ArgStringList &CmdArgs,
                         bool LinkBinaries) const;

    /// The HasGoldPass arguments tells the function if
    /// we will execute gold or if linking with ELFs should throw an error.
    /// Return the
    const char * AddInputFiles(const llvm::opt::ArgList &Args,
                       const std::vector<std::string> &LibPaths,
                       const InputInfoList &Inputs,
                       ArgStringList &LinkInputs, ArgStringList &GoldInputs,
                       const char *linkedBCFileName,
                       unsigned &linkedOFileInputPos,
                       bool AddLibSyms, bool LinkLibraries,
                       bool HasGoldPass, bool UseLTO) const;

    /// Return true if any options have been added to LinkInputs.
    bool AddSystemLibrary(const llvm::opt::ArgList &Args,
                          const std::vector<std::string> &LibPaths,
                          ArgStringList &LinkInputs, ArgStringList &GoldInputs,
                          const char *libo, const char *libflag,
                          bool AddLibSyms, bool HasGoldPass, bool UseLTO) const;

    /// Add arguments to link with libc, librt, librtsf, libpatmos
    void AddStandardLibs(const llvm::opt::ArgList &Args,
                         const std::vector<std::string> &LibPaths,
                         ArgStringList &LinkInputs, ArgStringList &GoldInputs,
                         bool AddRuntimeLibs, bool AddLibGloss, bool AddLibC,
                         bool AddLibSyms, StringRef FloatABI,
                         bool HasGoldPass, bool UseLTO) const;


    /// Returns linkedBCFileName if files need to be linked, or the filename of
    /// the only bitcode input file if there is no need to link, or null if
    /// there are no bitcode inputs.
    /// @linkedOFileInsertPos - position in GoldInputs where to insert the
    /// compiled bitcode file into.
    const char * PrepareLinkerInputs(const llvm::opt::ArgList &Args,
                         const InputInfoList &Inputs,
                         llvm::opt::ArgStringList &LinkInputs,
                         llvm::opt::ArgStringList &GoldInputs,
                         const char *linkedBCFileName,
                         unsigned &linkedOFileInsertPos,
                         bool AddStartFiles,
                         bool AddRuntimeLibs, bool AddLibGloss, bool AddLibC,
                         bool AddLibSyms, StringRef FloatABI,
                         bool LinkLibraries,
                         bool HasGoldPass, bool UseLTO) const;

    void ConstructLinkJob(const Tool &Creator, Compilation &C,
                          const JobAction &JA,
                          const char *OutputFilename,
                          const llvm::opt::ArgStringList &LinkInputs,
                          const llvm::opt::ArgList &TCArgs) const;

    // Construct an optimization job
    // @IsLinkPass - If true, add standard link optimizations
    bool ConstructOptJob(const Tool &Creator, Compilation &C,
                         const JobAction &JA,
                         const char *OutputFilename,
                         const char *InputFilename,
                         const llvm::opt::ArgList &TCArgs,
                         bool IsLinkPass, bool LinkAsObject, bool IsLastPass) const;

    void ConstructLLCJob(const Tool &Creator, Compilation &C,
                      const JobAction &JA,
                      const char *OutputFilename,
                      const char *InputFilename,
                      const llvm::opt::ArgList &TCArgs,
                      bool EmitAsm) const;

    void ConstructGoldJob(const Tool &Creator, Compilation &C,
                          const JobAction &JA,
                          const char *OutputFilename,
                          const llvm::opt::ArgStringList &GoldInputs,
                          const llvm::opt::ArgList &TCArgs, bool UseLTO,
                          bool LinkRelocatable, bool AddStackSymbols) const;
  };

  class LLVM_LIBRARY_VISIBILITY Compile : public Clang, protected PatmosBaseTool
  {
  public:
    Compile(const ToolChain &TC) : Clang(TC), PatmosBaseTool(TC) {}

    virtual void ConstructJob(Compilation &C, const JobAction &JA,
                              const InputInfo &Output,
                              const InputInfoList &Inputs,
                              const llvm::opt::ArgList &TCArgs,
                              const char *LinkingOutput) const;

    virtual bool hasIntegratedAssembler() const { return false; }
  };

  class LLVM_LIBRARY_VISIBILITY Link : public Tool, protected PatmosBaseTool {
  public:
    Link(const ToolChain &TC) : Tool("patmos::Link",
                                     "link  via llvm-link, opt and llc", TC),
                                PatmosBaseTool(TC) {}

    virtual void ConstructJob(Compilation &C, const JobAction &JA,
                              const InputInfo &Output,
                              const InputInfoList &Inputs,
                              const llvm::opt::ArgList &TCArgs,
                              const char *LinkingOutput) const;

    virtual bool hasIntegratedCPP() const { return false; }
    virtual bool isLinkJob() const { return true; }

  };
}

  /// gcc - Generic GCC tool implementations.
namespace gcc {
  class LLVM_LIBRARY_VISIBILITY Common : public Tool {
  public:
    Common(const char *Name, const char *ShortName,
           const ToolChain &TC) : Tool(Name, ShortName, TC) {}

    virtual void ConstructJob(Compilation &C, const JobAction &JA,
                              const InputInfo &Output,
                              const InputInfoList &Inputs,
                              const llvm::opt::ArgList &TCArgs,
                              const char *LinkingOutput) const;

    /// RenderExtraToolArgs - Render any arguments necessary to force
    /// the particular tool mode.
    virtual void
        RenderExtraToolArgs(const JobAction &JA,
                            llvm::opt::ArgStringList &CmdArgs) const = 0;
  };

  class LLVM_LIBRARY_VISIBILITY Preprocess : public Common {
  public:
    Preprocess(const ToolChain &TC) : Common("gcc::Preprocess",
                                             "gcc preprocessor", TC) {}

    virtual bool hasGoodDiagnostics() const { return true; }
    virtual bool hasIntegratedCPP() const { return false; }

    virtual void RenderExtraToolArgs(const JobAction &JA,
                                     llvm::opt::ArgStringList &CmdArgs) const;
  };

  class LLVM_LIBRARY_VISIBILITY Precompile : public Common  {
  public:
    Precompile(const ToolChain &TC) : Common("gcc::Precompile",
                                             "gcc precompile", TC) {}

    virtual bool hasGoodDiagnostics() const { return true; }
    virtual bool hasIntegratedCPP() const { return true; }

    virtual void RenderExtraToolArgs(const JobAction &JA,
                                     llvm::opt::ArgStringList &CmdArgs) const;
  };

  class LLVM_LIBRARY_VISIBILITY Compile : public Common  {
  public:
    Compile(const ToolChain &TC) : Common("gcc::Compile",
                                          "gcc frontend", TC) {}

    virtual bool hasGoodDiagnostics() const { return true; }
    virtual bool hasIntegratedCPP() const { return true; }

    virtual void RenderExtraToolArgs(const JobAction &JA,
                                     llvm::opt::ArgStringList &CmdArgs) const;
  };

  class LLVM_LIBRARY_VISIBILITY Assemble : public Common  {
  public:
    Assemble(const ToolChain &TC) : Common("gcc::Assemble",
                                           "assembler (via gcc)", TC) {}

    virtual bool hasIntegratedCPP() const { return false; }

    virtual void RenderExtraToolArgs(const JobAction &JA,
                                     llvm::opt::ArgStringList &CmdArgs) const;
  };

  class LLVM_LIBRARY_VISIBILITY Link : public Common  {
  public:
    Link(const ToolChain &TC) : Common("gcc::Link",
                                       "linker (via gcc)", TC) {}

    virtual bool hasIntegratedCPP() const { return false; }
    virtual bool isLinkJob() const { return true; }

    virtual void RenderExtraToolArgs(const JobAction &JA,
                                     llvm::opt::ArgStringList &CmdArgs) const;
  };
} // end namespace gcc

namespace hexagon {
  // For Hexagon, we do not need to instantiate tools for PreProcess, PreCompile and Compile.
  // We simply use "clang -cc1" for those actions.
  class LLVM_LIBRARY_VISIBILITY Assemble : public Tool {
  public:
    Assemble(const ToolChain &TC) : Tool("hexagon::Assemble",
      "hexagon-as", TC) {}

    virtual bool hasIntegratedCPP() const { return false; }

    virtual void RenderExtraToolArgs(const JobAction &JA,
                                     llvm::opt::ArgStringList &CmdArgs) const;
    virtual void ConstructJob(Compilation &C, const JobAction &JA,
                              const InputInfo &Output,
                              const InputInfoList &Inputs,
                              const llvm::opt::ArgList &TCArgs,
                              const char *LinkingOutput) const;
  };

  class LLVM_LIBRARY_VISIBILITY Link : public Tool {
  public:
    Link(const ToolChain &TC) : Tool("hexagon::Link",
      "hexagon-ld", TC) {}

    virtual bool hasIntegratedCPP() const { return false; }
    virtual bool isLinkJob() const { return true; }

    virtual void RenderExtraToolArgs(const JobAction &JA,
                                     llvm::opt::ArgStringList &CmdArgs) const;
    virtual void ConstructJob(Compilation &C, const JobAction &JA,
                              const InputInfo &Output,
                              const InputInfoList &Inputs,
                              const llvm::opt::ArgList &TCArgs,
                              const char *LinkingOutput) const;
  };
} // end namespace hexagon.


namespace darwin {
  llvm::Triple::ArchType getArchTypeForDarwinArchName(StringRef Str);

  class LLVM_LIBRARY_VISIBILITY DarwinTool : public Tool {
    virtual void anchor();
  protected:
    void AddDarwinArch(const llvm::opt::ArgList &Args,
                       llvm::opt::ArgStringList &CmdArgs) const;

    const toolchains::Darwin &getDarwinToolChain() const {
      return reinterpret_cast<const toolchains::Darwin&>(getToolChain());
    }

  public:
    DarwinTool(const char *Name, const char *ShortName,
               const ToolChain &TC) : Tool(Name, ShortName, TC) {}
  };

  class LLVM_LIBRARY_VISIBILITY Assemble : public DarwinTool  {
  public:
    Assemble(const ToolChain &TC) : DarwinTool("darwin::Assemble",
                                               "assembler", TC) {}

    virtual bool hasIntegratedCPP() const { return false; }

    virtual void ConstructJob(Compilation &C, const JobAction &JA,
                              const InputInfo &Output,
                              const InputInfoList &Inputs,
                              const llvm::opt::ArgList &TCArgs,
                              const char *LinkingOutput) const;
  };

  class LLVM_LIBRARY_VISIBILITY Link : public DarwinTool  {
    bool NeedsTempPath(const InputInfoList &Inputs) const;
    void AddLinkArgs(Compilation &C, const llvm::opt::ArgList &Args,
                     llvm::opt::ArgStringList &CmdArgs,
                     const InputInfoList &Inputs) const;

  public:
    Link(const ToolChain &TC) : DarwinTool("darwin::Link", "linker", TC) {}

    virtual bool hasIntegratedCPP() const { return false; }
    virtual bool isLinkJob() const { return true; }

    virtual void ConstructJob(Compilation &C, const JobAction &JA,
                              const InputInfo &Output,
                              const InputInfoList &Inputs,
                              const llvm::opt::ArgList &TCArgs,
                              const char *LinkingOutput) const;
  };

  class LLVM_LIBRARY_VISIBILITY Lipo : public DarwinTool  {
  public:
    Lipo(const ToolChain &TC) : DarwinTool("darwin::Lipo", "lipo", TC) {}

    virtual bool hasIntegratedCPP() const { return false; }

    virtual void ConstructJob(Compilation &C, const JobAction &JA,
                              const InputInfo &Output,
                              const InputInfoList &Inputs,
                              const llvm::opt::ArgList &TCArgs,
                              const char *LinkingOutput) const;
  };

  class LLVM_LIBRARY_VISIBILITY Dsymutil : public DarwinTool  {
  public:
    Dsymutil(const ToolChain &TC) : DarwinTool("darwin::Dsymutil",
                                               "dsymutil", TC) {}

    virtual bool hasIntegratedCPP() const { return false; }
    virtual bool isDsymutilJob() const { return true; }

    virtual void ConstructJob(Compilation &C, const JobAction &JA,
                              const InputInfo &Output,
                              const InputInfoList &Inputs,
                              const llvm::opt::ArgList &TCArgs,
                              const char *LinkingOutput) const;
  };

  class LLVM_LIBRARY_VISIBILITY VerifyDebug : public DarwinTool  {
  public:
    VerifyDebug(const ToolChain &TC) : DarwinTool("darwin::VerifyDebug",
                                                  "dwarfdump", TC) {}

    virtual bool hasIntegratedCPP() const { return false; }

    virtual void ConstructJob(Compilation &C, const JobAction &JA,
                              const InputInfo &Output,
                              const InputInfoList &Inputs,
                              const llvm::opt::ArgList &TCArgs,
                              const char *LinkingOutput) const;
  };

}

  /// openbsd -- Directly call GNU Binutils assembler and linker
namespace openbsd {
  class LLVM_LIBRARY_VISIBILITY Assemble : public Tool  {
  public:
    Assemble(const ToolChain &TC) : Tool("openbsd::Assemble", "assembler",
                                         TC) {}

    virtual bool hasIntegratedCPP() const { return false; }

    virtual void ConstructJob(Compilation &C, const JobAction &JA,
                              const InputInfo &Output,
                              const InputInfoList &Inputs,
                              const llvm::opt::ArgList &TCArgs,
                              const char *LinkingOutput) const;
  };
  class LLVM_LIBRARY_VISIBILITY Link : public Tool  {
  public:
    Link(const ToolChain &TC) : Tool("openbsd::Link", "linker", TC) {}

    virtual bool hasIntegratedCPP() const { return false; }
    virtual bool isLinkJob() const { return true; }

    virtual void ConstructJob(Compilation &C, const JobAction &JA,
                              const InputInfo &Output,
                              const InputInfoList &Inputs,
                              const llvm::opt::ArgList &TCArgs,
                              const char *LinkingOutput) const;
  };
} // end namespace openbsd

  /// bitrig -- Directly call GNU Binutils assembler and linker
namespace bitrig {
  class LLVM_LIBRARY_VISIBILITY Assemble : public Tool  {
  public:
    Assemble(const ToolChain &TC) : Tool("bitrig::Assemble", "assembler",
                                         TC) {}

    virtual bool hasIntegratedCPP() const { return false; }

    virtual void ConstructJob(Compilation &C, const JobAction &JA,
                              const InputInfo &Output,
                              const InputInfoList &Inputs,
                              const llvm::opt::ArgList &TCArgs,
                              const char *LinkingOutput) const;
  };
  class LLVM_LIBRARY_VISIBILITY Link : public Tool  {
  public:
    Link(const ToolChain &TC) : Tool("bitrig::Link", "linker", TC) {}

    virtual bool hasIntegratedCPP() const { return false; }
    virtual bool isLinkJob() const { return true; }

    virtual void ConstructJob(Compilation &C, const JobAction &JA,
                              const InputInfo &Output,
                              const InputInfoList &Inputs,
                              const llvm::opt::ArgList &TCArgs,
                              const char *LinkingOutput) const;
  };
} // end namespace bitrig

  /// freebsd -- Directly call GNU Binutils assembler and linker
namespace freebsd {
  class LLVM_LIBRARY_VISIBILITY Assemble : public Tool  {
  public:
    Assemble(const ToolChain &TC) : Tool("freebsd::Assemble", "assembler",
                                         TC) {}

    virtual bool hasIntegratedCPP() const { return false; }

    virtual void ConstructJob(Compilation &C, const JobAction &JA,
                              const InputInfo &Output,
                              const InputInfoList &Inputs,
                              const llvm::opt::ArgList &TCArgs,
                              const char *LinkingOutput) const;
  };
  class LLVM_LIBRARY_VISIBILITY Link : public Tool  {
  public:
    Link(const ToolChain &TC) : Tool("freebsd::Link", "linker", TC) {}

    virtual bool hasIntegratedCPP() const { return false; }
    virtual bool isLinkJob() const { return true; }

    virtual void ConstructJob(Compilation &C, const JobAction &JA,
                              const InputInfo &Output,
                              const InputInfoList &Inputs,
                              const llvm::opt::ArgList &TCArgs,
                              const char *LinkingOutput) const;
  };
} // end namespace freebsd

  /// netbsd -- Directly call GNU Binutils assembler and linker
namespace netbsd {
  class LLVM_LIBRARY_VISIBILITY Assemble : public Tool  {

  public:
    Assemble(const ToolChain &TC)
      : Tool("netbsd::Assemble", "assembler", TC) {}

    virtual bool hasIntegratedCPP() const { return false; }

    virtual void ConstructJob(Compilation &C, const JobAction &JA,
                              const InputInfo &Output,
                              const InputInfoList &Inputs,
                              const llvm::opt::ArgList &TCArgs,
                              const char *LinkingOutput) const;
  };
  class LLVM_LIBRARY_VISIBILITY Link : public Tool  {

  public:
    Link(const ToolChain &TC)
      : Tool("netbsd::Link", "linker", TC) {}

    virtual bool hasIntegratedCPP() const { return false; }
    virtual bool isLinkJob() const { return true; }

    virtual void ConstructJob(Compilation &C, const JobAction &JA,
                              const InputInfo &Output,
                              const InputInfoList &Inputs,
                              const llvm::opt::ArgList &TCArgs,
                              const char *LinkingOutput) const;
  };
} // end namespace netbsd

  /// Directly call GNU Binutils' assembler and linker.
namespace gnutools {
  class LLVM_LIBRARY_VISIBILITY Assemble : public Tool  {
  public:
    Assemble(const ToolChain &TC) : Tool("GNU::Assemble", "assembler", TC) {}

    virtual bool hasIntegratedCPP() const { return false; }

    virtual void ConstructJob(Compilation &C, const JobAction &JA,
                              const InputInfo &Output,
                              const InputInfoList &Inputs,
                              const llvm::opt::ArgList &TCArgs,
                              const char *LinkingOutput) const;
  };
  class LLVM_LIBRARY_VISIBILITY Link : public Tool  {
  public:
    Link(const ToolChain &TC) : Tool("GNU::Link", "linker", TC) {}

    virtual bool hasIntegratedCPP() const { return false; }
    virtual bool isLinkJob() const { return true; }

    virtual void ConstructJob(Compilation &C, const JobAction &JA,
                              const InputInfo &Output,
                              const InputInfoList &Inputs,
                              const llvm::opt::ArgList &TCArgs,
                              const char *LinkingOutput) const;
  };
}
  /// minix -- Directly call GNU Binutils assembler and linker
namespace minix {
  class LLVM_LIBRARY_VISIBILITY Assemble : public Tool  {
  public:
    Assemble(const ToolChain &TC) : Tool("minix::Assemble", "assembler",
                                         TC) {}

    virtual bool hasIntegratedCPP() const { return false; }

    virtual void ConstructJob(Compilation &C, const JobAction &JA,
                              const InputInfo &Output,
                              const InputInfoList &Inputs,
                              const llvm::opt::ArgList &TCArgs,
                              const char *LinkingOutput) const;
  };
  class LLVM_LIBRARY_VISIBILITY Link : public Tool  {
  public:
    Link(const ToolChain &TC) : Tool("minix::Link", "linker", TC) {}

    virtual bool hasIntegratedCPP() const { return false; }
    virtual bool isLinkJob() const { return true; }

    virtual void ConstructJob(Compilation &C, const JobAction &JA,
                              const InputInfo &Output,
                              const InputInfoList &Inputs,
                              const llvm::opt::ArgList &TCArgs,
                              const char *LinkingOutput) const;
  };
} // end namespace minix

  /// solaris -- Directly call Solaris assembler and linker
namespace solaris {
  class LLVM_LIBRARY_VISIBILITY Assemble : public Tool  {
  public:
    Assemble(const ToolChain &TC) : Tool("solaris::Assemble", "assembler",
                                         TC) {}

    virtual bool hasIntegratedCPP() const { return false; }

    virtual void ConstructJob(Compilation &C, const JobAction &JA,
                              const InputInfo &Output,
                              const InputInfoList &Inputs,
                              const llvm::opt::ArgList &TCArgs,
                              const char *LinkingOutput) const;
  };
  class LLVM_LIBRARY_VISIBILITY Link : public Tool  {
  public:
    Link(const ToolChain &TC) : Tool("solaris::Link", "linker", TC) {}

    virtual bool hasIntegratedCPP() const { return false; }
    virtual bool isLinkJob() const { return true; }

    virtual void ConstructJob(Compilation &C, const JobAction &JA,
                              const InputInfo &Output,
                              const InputInfoList &Inputs,
                              const llvm::opt::ArgList &TCArgs,
                              const char *LinkingOutput) const;
  };
} // end namespace solaris

  /// auroraux -- Directly call GNU Binutils assembler and linker
namespace auroraux {
  class LLVM_LIBRARY_VISIBILITY Assemble : public Tool  {
  public:
    Assemble(const ToolChain &TC) : Tool("auroraux::Assemble", "assembler",
                                         TC) {}

    virtual bool hasIntegratedCPP() const { return false; }

    virtual void ConstructJob(Compilation &C, const JobAction &JA,
                              const InputInfo &Output,
                              const InputInfoList &Inputs,
                              const llvm::opt::ArgList &TCArgs,
                              const char *LinkingOutput) const;
  };
  class LLVM_LIBRARY_VISIBILITY Link : public Tool  {
  public:
    Link(const ToolChain &TC) : Tool("auroraux::Link", "linker", TC) {}

    virtual bool hasIntegratedCPP() const { return false; }
    virtual bool isLinkJob() const { return true; }

    virtual void ConstructJob(Compilation &C, const JobAction &JA,
                              const InputInfo &Output,
                              const InputInfoList &Inputs,
                              const llvm::opt::ArgList &TCArgs,
                              const char *LinkingOutput) const;
  };
} // end namespace auroraux

  /// dragonfly -- Directly call GNU Binutils assembler and linker
namespace dragonfly {
  class LLVM_LIBRARY_VISIBILITY Assemble : public Tool  {
  public:
    Assemble(const ToolChain &TC) : Tool("dragonfly::Assemble", "assembler",
                                         TC) {}

    virtual bool hasIntegratedCPP() const { return false; }

    virtual void ConstructJob(Compilation &C, const JobAction &JA,
                              const InputInfo &Output,
                              const InputInfoList &Inputs,
                              const llvm::opt::ArgList &TCArgs,
                              const char *LinkingOutput) const;
  };
  class LLVM_LIBRARY_VISIBILITY Link : public Tool  {
  public:
    Link(const ToolChain &TC) : Tool("dragonfly::Link", "linker", TC) {}

    virtual bool hasIntegratedCPP() const { return false; }
    virtual bool isLinkJob() const { return true; }

    virtual void ConstructJob(Compilation &C, const JobAction &JA,
                              const InputInfo &Output,
                              const InputInfoList &Inputs,
                              const llvm::opt::ArgList &TCArgs,
                              const char *LinkingOutput) const;
  };
} // end namespace dragonfly

  /// Visual studio tools.
namespace visualstudio {
  class LLVM_LIBRARY_VISIBILITY Link : public Tool {
  public:
    Link(const ToolChain &TC) : Tool("visualstudio::Link", "linker", TC) {}

    virtual bool hasIntegratedCPP() const { return false; }
    virtual bool isLinkJob() const { return true; }

    virtual void ConstructJob(Compilation &C, const JobAction &JA,
                              const InputInfo &Output,
                              const InputInfoList &Inputs,
                              const llvm::opt::ArgList &TCArgs,
                              const char *LinkingOutput) const;
  };

  class LLVM_LIBRARY_VISIBILITY Compile : public Tool {
  public:
    Compile(const ToolChain &TC) : Tool("visualstudio::Compile", "compiler", TC) {}

    virtual bool hasIntegratedAssembler() const { return true; }
    virtual bool hasIntegratedCPP() const { return true; }
    virtual bool isLinkJob() const { return false; }

    virtual void ConstructJob(Compilation &C, const JobAction &JA,
                              const InputInfo &Output,
                              const InputInfoList &Inputs,
                              const llvm::opt::ArgList &TCArgs,
                              const char *LinkingOutput) const;

    Command *GetCommand(Compilation &C, const JobAction &JA,
                        const InputInfo &Output,
                        const InputInfoList &Inputs,
                        const llvm::opt::ArgList &TCArgs,
                        const char *LinkingOutput) const;
  };
} // end namespace visualstudio

namespace XCore {
  // For XCore, we do not need to instantiate tools for PreProcess, PreCompile and Compile.
  // We simply use "clang -cc1" for those actions.
  class LLVM_LIBRARY_VISIBILITY Assemble : public Tool {
  public:
    Assemble(const ToolChain &TC) : Tool("XCore::Assemble",
      "XCore-as", TC) {}

    virtual bool hasIntegratedCPP() const { return false; }
    virtual void ConstructJob(Compilation &C, const JobAction &JA,
                              const InputInfo &Output,
                              const InputInfoList &Inputs,
                              const llvm::opt::ArgList &TCArgs,
                              const char *LinkingOutput) const;
  };

  class LLVM_LIBRARY_VISIBILITY Link : public Tool {
  public:
    Link(const ToolChain &TC) : Tool("XCore::Link",
      "XCore-ld", TC) {}

    virtual bool hasIntegratedCPP() const { return false; }
    virtual bool isLinkJob() const { return true; }
    virtual void ConstructJob(Compilation &C, const JobAction &JA,
                              const InputInfo &Output,
                              const InputInfoList &Inputs,
                              const llvm::opt::ArgList &TCArgs,
                              const char *LinkingOutput) const;
  };
} // end namespace XCore.


} // end namespace toolchains
} // end namespace driver
} // end namespace clang

#endif // CLANG_LIB_DRIVER_TOOLS_H_
