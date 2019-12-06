#pragma once

#include "CAFAna/Prediction/IPrediction.h"
#include "CAFAna/Prediction/PredictionGenerator.h"
#include "CAFAna/Prediction/PredictionInterpKernel.h"

#include "CAFAna/Core/SpectrumLoader.h"
#include "CAFAna/Core/SystShifts.h"

#include <map>
#include <memory>

#include "TMD5.h"

class TH1;

namespace ana
{
  class Loaders;

  /// Implements systematic errors by interpolation between shifted templates
  class PredictionInterp: public IPrediction
  {
  public:
    enum EMode_t{
      kCombineSigns, kSplitBySign
    };

    /// \param systs What systematics we should be capable of interpolating
    /// \param osc The oscillation point to expand around
    /// \param predGen Construct an IPrediction from the following
    ///                information.
    /// \param loaders The loaders to be passed on to the underlying prediction
    /// \param shiftMC Underlying shift. Use with care. Mostly for
    ///                PredictionNumuFAHadE. Should not contain any of of
    ///                \a systs
    /// \param mode    In FHC the wrong-sign has bad stats and probably the
    ///                fits can't be split out reasonably. For RHC it's
    ///                important not to conflate them.
    PredictionInterp(std::vector<const ISyst*> systs,
                     osc::IOscCalculator* osc,
                     const IPredictionGenerator& predGen,
                     Loaders& loaders,
                     const SystShifts& shiftMC = kNoShift,
                     EMode_t mode = kCombineSigns);

    virtual ~PredictionInterp();



    Spectrum Predict(osc::IOscCalculator* calc) const override;
    SpectrumStan Predict(osc::IOscCalculatorStan* calc) const override;


    Spectrum PredictSyst(osc::IOscCalculator* calc,
                         const SystShifts& shift) const override;
    SpectrumStan PredictSyst(osc::IOscCalculatorStan* calc,
                             const SystShifts& shift) const override;

    Spectrum PredictComponent(osc::IOscCalculator* calc,
                              Flavors::Flavors_t flav,
                              Current::Current_t curr,
                              Sign::Sign_t sign) const override;
    SpectrumStan PredictComponent(osc::IOscCalculatorStan* calc,
                                  Flavors::Flavors_t flav,
                                  Current::Current_t curr,
                                  Sign::Sign_t sign) const override;

    Spectrum PredictComponentSyst(osc::IOscCalculator* calc,
                                  const SystShifts& shift,
                                  Flavors::Flavors_t flav,
                                  Current::Current_t curr,
                                  Sign::Sign_t sign) const override;
    SpectrumStan PredictComponentSyst(osc::IOscCalculatorStan* calc,
                                      const SystShifts& shift,
                                      Flavors::Flavors_t flav,
                                      Current::Current_t curr,
                                      Sign::Sign_t sign) const override;

    void Derivative(osc::IOscCalculator* calc,
                    const SystShifts& shift,
                    double pot,
                    std::unordered_map<const ISyst*, std::vector<double>>& dp) const override;

    virtual void SaveTo(TDirectory* dir) const override;
    static std::unique_ptr<PredictionInterp> LoadFrom(TDirectory* dir);

    /// After calling this DebugPlots won't work fully and SaveTo won't work at
    /// all.
    void MinimizeMemory();

    void DebugPlot(const ISyst* syst,
                   osc::IOscCalculator* calc,
                   Flavors::Flavors_t flav = Flavors::kAll,
                   Current::Current_t curr = Current::kBoth,
                   Sign::Sign_t sign = Sign::kBoth) const;

    // If \a savePattern is not empty, print each pad. Must contain a "%s" to
    // contain the name of the systematic.
    void DebugPlots(osc::IOscCalculator* calc,
		    const std::string& savePattern = "",
		    Flavors::Flavors_t flav = Flavors::kAll,
		    Current::Current_t curr = Current::kBoth,
		    Sign::Sign_t sign = Sign::kBoth) const;

    void SetOscSeed(osc::IOscCalculator* oscSeed);

    void DebugPlotColz(const ISyst* syst,
                       osc::IOscCalculator* calc,
                       Flavors::Flavors_t flav = Flavors::kAll,
                       Current::Current_t curr = Current::kBoth,
                       Sign::Sign_t sign = Sign::kBoth) const;

    void DebugPlotsColz(osc::IOscCalculator* calc,
                        const std::string& savePattern = "",
                        Flavors::Flavors_t flav = Flavors::kAll,
                        Current::Current_t curr = Current::kBoth,
                        Sign::Sign_t sign = Sign::kBoth) const;

    bool SplitBySign() const {return fSplitBySign;}
    enum CoeffsType{
      kNueApp, kNueSurv, kNumuSurv, kNC,
      kOther, ///< Taus, numu appearance
      kNCoeffTypes
    };

    PredictionInterp() : fOscOrigin(nullptr), fBinning(0, {}, {}, 0, 0), fSplitBySign(false) {
      if(getenv("CAFANA_PRED_MINMCSTATS")){
        fMinMCStats = atoi(getenv("CAFANA_PRED_MINMCSTATS"));
      } else {
        fMinMCStats = 50;
      }
    }

    static void LoadFromBody(TDirectory* dir, PredictionInterp* ret,
			     std::vector<const ISyst*> veto = {});

    typedef ana::PredIntKern::Coeffs Coeffs;

    /// Find coefficients describing this set of shifts
    std::vector<std::vector<Coeffs>>
    FitRatios(const std::vector<double>& shifts,
              const std::vector<std::unique_ptr<TH1>>& ratios) const;

    /// Find coefficients describing the ratios from this component
    std::vector<std::vector<Coeffs>>
    FitComponent(const std::vector<double>& shifts,
                 const std::vector<std::unique_ptr<IPrediction>>& preds,
                 Flavors::Flavors_t flav,
                 Current::Current_t curr,
                 Sign::Sign_t sign,
                 const std::string& systName) const;

    Spectrum ShiftSpectrum(const Spectrum& s,
                           CoeffsType type,
                           bool nubar, // try to use fitsNubar if it exists
                           const SystShifts& shift) const;
      SpectrumStan ShiftSpectrum(const SpectrumStan& s,
                                 CoeffsType type,
                                 bool nubar, // try to use fitsNubar if it exists
                                 const SystShifts& shift) const;

    /// Helper for PredictComponentSyst
    Spectrum ShiftedComponent(osc::IOscCalculator* calc,
                              const TMD5* hash,
                              const SystShifts& shift,
                              Flavors::Flavors_t flav,
                              Current::Current_t curr,
                              Sign::Sign_t sign,
                              CoeffsType type) const;

      SpectrumStan ShiftedComponent(osc::_IOscCalculator<stan::math::var>* calc,
                                    const TMD5* hash,
                                    const SystShifts& shift,
                                    Flavors::Flavors_t flav,
                                    Current::Current_t curr,
                                    Sign::Sign_t sign,
                                    CoeffsType type) const;

      //Memory saving feature, if you know you wont need any systs that were loaded in, can discard them.
      void DiscardSysts(std::vector<ISyst const *>const &);
      //Get all known about systs
      std::vector<ISyst const *> GetAllSysts() const;

    protected:
      std::unique_ptr<IPrediction> fPredNom; ///< The nominal prediction

      struct ShiftedPreds
      {
        double Stride() const {return shifts.size() > 1 ? shifts[1]-shifts[0] : 1;}

        std::string systName; ///< What systematic we're interpolating
        std::vector<double> shifts; ///< Shift values sampled
        std::vector<std::unique_ptr<IPrediction>> preds;

        int nCoeffs; // Faster than calling size()

        /// Indices: [type][histogram bin][shift bin]
        std::vector<std::vector<std::vector<Coeffs>>> fits;
        /// Will be filled if signs are separated, otherwise not
        std::vector<std::vector<std::vector<Coeffs>>> fitsNubar;

        // Same info as above but with more-easily-iterable index order
        // [type][shift bin][histogram bin]. TODO this is ugly
        std::vector<std::vector<std::vector<Coeffs>>> fitsRemap;
        std::vector<std::vector<std::vector<Coeffs>>> fitsNubarRemap;
        ShiftedPreds() {}
        ShiftedPreds(ShiftedPreds &&other)
            : systName(std::move(other.systName)),
              shifts(std::move(other.shifts)), preds(std::move(other.preds)),
              nCoeffs(other.nCoeffs), fits(std::move(other.fits)),
              fitsNubar(std::move(other.fitsNubar)),
              fitsRemap(std::move(other.fitsRemap)),
              fitsNubarRemap(std::move(other.fitsNubarRemap)) {}

        ShiftedPreds &operator=(ShiftedPreds &&other) {
          systName = std::move(other.systName);
          shifts = std::move(other.shifts);
          preds = std::move(other.preds);
          nCoeffs = other.nCoeffs;
          fits = std::move(other.fits);
          fitsNubar = std::move(other.fitsNubar);
          fitsRemap = std::move(other.fitsRemap);
          fitsNubarRemap = std::move(other.fitsNubarRemap);
          return *this;
        }

        void Dump(){
          std::cout << "[INFO]: " << systName << ", with " << preds.size() << " preds." << std::endl;
        }
      };

    using PredMappedType = std::pair<const ISyst *, ShiftedPreds>;
    mutable std::vector<PredMappedType> fPreds;
    std::vector<PredMappedType>::iterator find_pred(const ISyst *s) const {
      return std::find_if(
          fPreds.begin(), fPreds.end(),
          [=](PredMappedType const &p) { return bool(s == p.first); });
    }
    ShiftedPreds const &get_pred(const ISyst *s) const {
      return std::find_if(fPreds.begin(), fPreds.end(),
                       [=](PredMappedType const &p) {
                         return bool(s == p.first);
                       })
          ->second;
    }

    /// The oscillation values we assume when evaluating the coefficients
    osc::IOscCalculator* fOscOrigin;

    mutable Spectrum fBinning; ///< Dummy spectrum to provide binning

    struct Key_t
    {
      Flavors::Flavors_t flav;
      Current::Current_t curr;
      Sign::Sign_t sign;
      bool operator<(const Key_t& rhs) const
      {
        return (std::make_tuple(flav, curr, sign) <
                std::make_tuple(rhs.flav, rhs.curr, rhs.sign));
      }
    };
    struct Val_t
    {
      TMD5 hash;
        Spectrum nom;  // todo: we can't cache stan::math::vars because they wind up getting invalidated when the Stan stack is cleared.  but keeping only a <double> version around in this cache means that we're dumping the autodiff for the oscillation calculator part, which may mean Stan won't explore the space correctly.  Not sure what to do here.
    };
    mutable std::map<Key_t, Val_t> fNomCache;

    bool fSplitBySign;

    // Don't apply systs to bins with fewer than this many MC stats
    double fMinMCStats;

    void InitFits() const;

    void InitFitsHelper(ShiftedPreds& sp,
                        std::vector<std::vector<std::vector<Coeffs>>>& fits,
                        Sign::Sign_t sign) const;

     /// Helper for \ref Derivative
    void ComponentDerivative(osc::IOscCalculator* calc,
                             Flavors::Flavors_t flav,
                             Current::Current_t curr,
                             Sign::Sign_t sign,
                             CoeffsType type,
                             const SystShifts& shift,
                             double pot,
                             std::unordered_map<const ISyst*, std::vector<double>>& dp) const;

    /// Templated helper for \ref ShiftedComponent
    template <typename U, typename T>
    U _ShiftedComponent(osc::_IOscCalculator<T>* calc,
                        const TMD5* hash,
                        const SystShifts& shift,
                        Flavors::Flavors_t flav,
                        Current::Current_t curr,
                        Sign::Sign_t sign,
                        CoeffsType type) const;

    /// Templated helper for \ref PredictComponentSyst
    template <typename U, typename T>
    U _PredictComponentSyst(osc::_IOscCalculator<T>* calc,
                            const SystShifts& shift,
                            Flavors::Flavors_t flav,
                            Current::Current_t curr,
                            Sign::Sign_t sign) const;

      /// Helper for \ref ShiftSpectrum
      template <typename T>
      void ShiftBins(unsigned int N,
                     T* arr,
                     CoeffsType type,
                     bool nubar,
                     const SystShifts& shift) const;
  };

}
