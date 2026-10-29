#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GameFramework/GameModeBase.h"
#include "GameFramework/HUD.h"
#include "Core/CaseSimulation.h"
#include "CaseGame.generated.h"

UCLASS()
class OTHERSIDE_API ACasePawn : public ACharacter
{
    GENERATED_BODY()
public:
    ACasePawn();
    virtual void SetupPlayerInputComponent(UInputComponent* Input) override;
    UPROPERTY() TObjectPtr<class UCameraComponent> Camera;
    UPROPERTY() TObjectPtr<class USpotLightComponent> Light;
private:
    void Forward(float V); void Right(float V); void Turn(float V); void Look(float V);
    void Interact(); void ToggleLight(); void Photo(); void Record(); void Board();
    void One(); void Two(); void Three(); void Quiet(); void Pause(); void Restart();
};

UCLASS()
class OTHERSIDE_API ACaseGameMode : public AGameModeBase
{
    GENERATED_BODY()
public:
    ACaseGameMode();
    virtual void BeginPlay() override;
    virtual void Tick(float Delta) override;
    OtherSide::CaseSimulation Case;
    bool bBoard=false, bPaused=false, bGentle=true;
    FString Message, Focus;
    float MessageTime=0;
    bool Blocked() const;
    void Action(int32 Kind);
    void Choose(int32 Choice);
    void Notify(const FString& Text);
    FString Objective() const;
private:
    AActor* Shape(FVector Position, FVector Scale, bool Sphere=false);
    void Label(FVector Position, const FString& Text);
    void Clue(FVector Position, const FString& Text, const FName& Tag);
    void BuildHouse();
    AActor* Target() const;
    UPROPERTY() TObjectPtr<AActor> Entity;
    UPROPERTY() TObjectPtr<AActor> MovingShelf;
    UPROPERTY() TObjectPtr<class USoundBase> Knock;
    UPROPERTY() TObjectPtr<class USoundAttenuation> WarningAttenuation;
    UPROPERTY() TObjectPtr<class UStaticMesh> CubeMesh;
    UPROPERTY() TObjectPtr<class UStaticMesh> SphereMesh;
    float Elapsed=0, Noise=0, SoundTime=0;
    FVector LastPlayer=FVector::ZeroVector;
    FVector Investigate=FVector::ZeroVector;
    bool bLayoutChanged=false;
};

UCLASS()
class OTHERSIDE_API ACaseHUD : public AHUD
{
    GENERATED_BODY()
public:
    virtual void DrawHUD() override;
};
