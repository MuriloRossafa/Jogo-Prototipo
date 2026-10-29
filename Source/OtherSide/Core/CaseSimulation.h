#pragma once
#include <algorithm>
#include <array>
#include <cstdint>
namespace OtherSide {
enum class Evidence : uint8_t { Registry, Photo, Recording, Testimony, Report, Count };
enum class Hypothesis : uint8_t { Fraud, TemporalLoop };
enum class Ending : uint8_t { None, Escaped, Contained, Released, Lost };
enum class Result : uint8_t { Accepted, Duplicate, MissingEvidence, WrongConclusion, WrongSequence, Finished };
struct CaseSimulation {
    std::array<bool, static_cast<unsigned>(Evidence::Count)> Clues{};
    float Fear=0, Battery=100, Exposure=0;
    bool Flashlight=true, Conclusion=false, TestimonyChallenged=false, DocumentRecovered=false;
    unsigned RitualStep=0;
    Ending Outcome=Ending::None;
    bool Has(Evidence e) const { return Clues[static_cast<unsigned>(e)]; }
    unsigned Count() const { unsigned n=0; for(bool v:Clues) n+=v; return n; }
    Result Collect(Evidence e) {
        if(Outcome!=Ending::None) return Result::Finished;
        if(e==Evidence::Count) return Result::MissingEvidence;
        if(e==Evidence::Report && !Conclusion) return Result::MissingEvidence;
        if(Has(e)) return Result::Duplicate;
        Clues[static_cast<unsigned>(e)]=true;
        if(e==Evidence::Report) DocumentRecovered=true;
        return Result::Accepted;
    }
    Result ChallengeWitness() {
        if(Outcome!=Ending::None) return Result::Finished;
        if(!Has(Evidence::Registry)||!Has(Evidence::Photo)||!Has(Evidence::Testimony)) return Result::MissingEvidence;
        if(TestimonyChallenged) return Result::Duplicate;
        TestimonyChallenged=true; Fear=std::min(100.f,Fear+8); return Result::Accepted;
    }
    Result Conclude(Hypothesis h) {
        if(Outcome!=Ending::None) return Result::Finished;
        if(Conclusion) return Result::Duplicate;
        if(!Has(Evidence::Recording)||!TestimonyChallenged) return Result::MissingEvidence;
        if(h!=Hypothesis::TemporalLoop) { Fear=std::min(100.f,Fear+10); return Result::WrongConclusion; }
        Conclusion=true; return Result::Accepted;
    }
    // The recording gives a fixed, discoverable order: name (1), date (2), silence (3).
    Result Ritual(unsigned symbol) {
        if(Outcome!=Ending::None) return Result::Finished;
        if(!DocumentRecovered) return Result::MissingEvidence;
        if(symbol!=RitualStep+1) { RitualStep=0; Fear=std::min(100.f,Fear+15); return Result::WrongSequence; }
        if(++RitualStep==3) Outcome=Ending::Contained;
        return Result::Accepted;
    }
    Result Release() {
        if(Outcome!=Ending::None) return Result::Finished;
        if(!DocumentRecovered) return Result::MissingEvidence;
        Outcome=Ending::Released; return Result::Accepted;
    }
    void Leave() { if(Outcome==Ending::None) Outcome=Ending::Escaped; }
    void Tick(float dt, bool danger, bool safe) {
        if(Outcome!=Ending::None || dt<=0) return;
        if(Flashlight) { Battery=std::max(0.f,Battery-dt*.12f); if(Battery==0) Flashlight=false; }
        Fear=std::clamp(Fear+dt*(danger?5.f:(safe?-5.f:-.7f)),0.f,100.f);
        Exposure=danger?Exposure+dt:std::max(0.f,Exposure-dt*2.f);
        if(Exposure>=6.f) Outcome=Ending::Lost;
    }
    void ToggleLight() { if(Outcome==Ending::None && Battery>0) Flashlight=!Flashlight; }
};
}
