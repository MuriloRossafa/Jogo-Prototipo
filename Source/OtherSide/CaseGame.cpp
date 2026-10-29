#include "CaseGame.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SpotLightComponent.h"
#include "Components/PointLightComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/TextRenderComponent.h"
#include "Engine/StaticMeshActor.h"
#include "Engine/PointLight.h"
#include "Engine/TextRenderActor.h"
#include "Engine/World.h"
#include "Engine/Engine.h"
#include "Engine/Canvas.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundBase.h"
#include "Sound/SoundAttenuation.h"
#include "Components/InputComponent.h"
#include "UObject/ConstructorHelpers.h"
using namespace OtherSide;
namespace {
ACaseGameMode* GM(const AActor* A) { return Cast<ACaseGameMode>(UGameplayStatics::GetGameMode(A)); }
const TCHAR* EvidenceNames[]={TEXT("Registro: o terreno está vazio desde 1978."),TEXT("Foto de 1978: a família desaparecida está na varanda."),TEXT("Fita: contenção na ordem NOME > DATA > SILÊNCIO."),TEXT("Depoimento: 'Moro aqui desde 1996. Nunca houve outra família.'"),TEXT("Relatório DOI de 1978: seu nome consta como responsável.")};
}
ACasePawn::ACasePawn() {
    GetCapsuleComponent()->InitCapsuleSize(32,88);
    Camera=CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
    Camera->SetupAttachment(GetCapsuleComponent()); Camera->SetRelativeLocation(FVector(0,0,64)); Camera->bUsePawnControlRotation=true;
    Light=CreateDefaultSubobject<USpotLightComponent>(TEXT("Flashlight")); Light->SetupAttachment(Camera);
    Light->SetIntensity(18000); Light->SetAttenuationRadius(1800); Light->SetOuterConeAngle(38); Light->SetInnerConeAngle(18);
    GetCharacterMovement()->MaxWalkSpeed=230;
}
void ACasePawn::SetupPlayerInputComponent(UInputComponent* I) {
    Super::SetupPlayerInputComponent(I);
    I->BindAxis("Forward",this,&ACasePawn::Forward); I->BindAxis("Right",this,&ACasePawn::Right);
    I->BindAxis("Turn",this,&ACasePawn::Turn); I->BindAxis("Look",this,&ACasePawn::Look);
    I->BindAction("Interact",IE_Pressed,this,&ACasePawn::Interact); I->BindAction("Light",IE_Pressed,this,&ACasePawn::ToggleLight);
    I->BindAction("Photo",IE_Pressed,this,&ACasePawn::Photo); I->BindAction("Record",IE_Pressed,this,&ACasePawn::Record);
    I->BindAction("Board",IE_Pressed,this,&ACasePawn::Board); I->BindAction("One",IE_Pressed,this,&ACasePawn::One);
    I->BindAction("Two",IE_Pressed,this,&ACasePawn::Two); I->BindAction("Three",IE_Pressed,this,&ACasePawn::Three);
    I->BindAction("Quiet",IE_Pressed,this,&ACasePawn::Quiet); I->BindAction("Pause",IE_Pressed,this,&ACasePawn::Pause);
    I->BindAction("Restart",IE_Pressed,this,&ACasePawn::Restart);
}
void ACasePawn::Forward(float V) { if(auto* G=GM(this); G&&!G->Blocked()) AddMovementInput(GetActorForwardVector(),V); }
void ACasePawn::Right(float V) { if(auto* G=GM(this); G&&!G->Blocked()) AddMovementInput(GetActorRightVector(),V); }
void ACasePawn::Turn(float V) { if(auto* G=GM(this); G&&!G->Blocked()) AddControllerYawInput(V); }
void ACasePawn::Look(float V) { if(auto* G=GM(this); G&&!G->Blocked()) AddControllerPitchInput(V); }
void ACasePawn::Interact(){ if(auto* G=GM(this)) G->Action(0); }
void ACasePawn::ToggleLight(){ if(auto* G=GM(this);G&&!G->Blocked()) G->Case.ToggleLight(); }
void ACasePawn::Photo(){ if(auto* G=GM(this)) G->Action(1); }
void ACasePawn::Record(){ if(auto* G=GM(this)) G->Action(2); }
void ACasePawn::Board(){ if(auto* G=GM(this);G&&!G->bPaused&&G->Case.Outcome==Ending::None) { G->bBoard=!G->bBoard; GetCharacterMovement()->StopMovementImmediately(); } }
void ACasePawn::One(){ if(auto* G=GM(this)) G->Choose(1); }
void ACasePawn::Two(){ if(auto* G=GM(this)) G->Choose(2); }
void ACasePawn::Three(){ if(auto* G=GM(this)) G->Choose(3); }
void ACasePawn::Quiet(){ if(auto* G=GM(this)) {G->bGentle=!G->bGentle;G->Notify(G->bGentle?TEXT("Atmosfera suave: sem pulsação de luz."):TEXT("Atmosfera intensa: pulsação lenta da lanterna sob ameaça."));} }
void ACasePawn::Pause(){ if(auto* G=GM(this)) {if(G->bBoard) G->bBoard=false; else G->bPaused=!G->bPaused; GetCharacterMovement()->StopMovementImmediately();} }
void ACasePawn::Restart(){ if(auto* G=GM(this);G&&G->Case.Outcome!=Ending::None) UGameplayStatics::OpenLevel(this,FName(*GetWorld()->GetName())); }
ACaseGameMode::ACaseGameMode() {
    static ConstructorHelpers::FObjectFinder<UStaticMesh> Cube(TEXT("/Engine/BasicShapes/Cube.Cube"));
    static ConstructorHelpers::FObjectFinder<UStaticMesh> Sphere(TEXT("/Engine/BasicShapes/Sphere.Sphere"));
    CubeMesh=Cube.Object; SphereMesh=Sphere.Object;
    PrimaryActorTick.bCanEverTick=true; DefaultPawnClass=ACasePawn::StaticClass(); HUDClass=ACaseHUD::StaticClass();
}
AActor* ACaseGameMode::Shape(FVector P,FVector S,bool Sphere) {
    auto* A=GetWorld()->SpawnActor<AStaticMeshActor>(P,FRotator::ZeroRotator);
    A->SetMobility(EComponentMobility::Movable);
    A->GetStaticMeshComponent()->SetStaticMesh(Sphere?SphereMesh.Get():CubeMesh.Get());
    A->SetActorScale3D(S); return A;
}
void ACaseGameMode::Label(FVector P,const FString& Text) {
    auto* A=GetWorld()->SpawnActor<ATextRenderActor>(P,FRotator(0,-90,0));
    A->GetTextRender()->SetText(FText::FromString(Text)); A->GetTextRender()->SetWorldSize(23);
    A->GetTextRender()->SetTextRenderColor(FColor(210,190,130));
}
void ACaseGameMode::Clue(FVector P,const FString& Text,const FName& Tag) {
    auto* A=Shape(P,FVector(.9,.9,.5)); A->Tags.Add(Tag); Label(P+FVector(-45,-55,70),Text);
}
void ACaseGameMode::BuildHouse() {
    Shape(FVector(0,350,-20),FVector(34,21,.4));
    Shape(FVector(0,350,340),FVector(34,21,.3));
    Shape(FVector(-1700,350,160),FVector(.3,21,3.6)); Shape(FVector(1700,350,160),FVector(.3,21,3.6));
    Shape(FVector(0,1400,160),FVector(34,.3,3.6)); Shape(FVector(0,-700,160),FVector(34,.3,3.6));
    for(float X:{-550.f,550.f}) Shape(FVector(X,900,160),FVector(.25,10,3.6));
    for(float X:{-1100.f,0.f,1100.f}) {
        // Doorways remain open; the changing shelf never blocks the route.
        Shape(FVector(X-340,400,160),FVector(4.2,.25,3.6)); Shape(FVector(X+340,400,160),FVector(4.2,.25,3.6));
        Shape(FVector(X,900,35),FVector(2.5,1.2,.7));
        auto* L=GetWorld()->SpawnActor<APointLight>(FVector(X,750,280),FRotator::ZeroRotator);
        if(auto* Point=L->FindComponentByClass<UPointLightComponent>()) {
            Point->SetMobility(EComponentMobility::Movable);
            Point->SetIntensity(3000); Point->SetAttenuationRadius(1100);
            Point->SetLightColor(FLinearColor(.72,.82,1));
        }
    }
    auto* Safe=GetWorld()->SpawnActor<APointLight>(FVector(-1200,-350,230),FRotator::ZeroRotator);
    if(auto* Point=Safe->FindComponentByClass<UPointLightComponent>()) {
        Point->SetMobility(EComponentMobility::Movable);
        Point->SetIntensity(5000); Point->SetAttenuationRadius(850);
        Point->SetLightColor(FLinearColor(1,.65,.3));
    }
    Label(FVector(-1550,-610,180),TEXT("DOI / SAÍDA\nAbrigo iluminado"));
    Clue(FVector(-1250,-480,70),TEXT("REGISTRO / E"),"Registry");
    Clue(FVector(-1100,850,115),TEXT("FOTOGRAFIA / C"),"Photo");
    Clue(FVector(0,850,115),TEXT("GRAVADOR / G"),"Recording");
    Clue(FVector(1100,850,115),TEXT("INTERFONE / E"),"Testimony");
    Clue(FVector(1450,1200,80),TEXT("COFRE DOI / E"),"Report");
    Clue(FVector(-1450,-580,70),TEXT("ENCERRAR MISSÃO / E"),"Exit");
    Clue(FVector(1400,-420,70),TEXT("SELO / E\n1 NOME  2 DATA  3 SILÊNCIO"),"Ritual");
    MovingShelf=Shape(FVector(1300,600,100),FVector(1,1.2,2));
    Entity=Shape(FVector(1100,1200,130),FVector(.65,.65,2.6),true);
    Entity->SetActorEnableCollision(false); Entity->SetActorHiddenInGame(true);
}
void ACaseGameMode::BeginPlay() {
    Super::BeginPlay(); BuildHouse();
    if(auto* P=Cast<ACasePawn>(UGameplayStatics::GetPlayerPawn(this,0))) {
        P->SetActorLocation(FVector(-1200,-300,100)); P->GetController()->SetControlRotation(FRotator(0,35,0)); LastPlayer=P->GetActorLocation();
    }
    WarningAttenuation=NewObject<USoundAttenuation>(this);
    WarningAttenuation->Attenuation.bAttenuate=true;
    WarningAttenuation->Attenuation.bSpatialize=true;
    WarningAttenuation->Attenuation.AttenuationShapeExtents=FVector(150,0,0);
    WarningAttenuation->Attenuation.FalloffDistance=1600;
    Knock=LoadObject<USoundBase>(nullptr,TEXT("/Game/Audio/Warning.Warning"));
    Notify(TEXT("Caso 001: entre na casa, confronte as datas e recupere o relatório. TAB abre o quadro. O abrigo junto à saída é seguro."));
}
bool ACaseGameMode::Blocked() const { return bPaused||bBoard||Case.Outcome!=Ending::None; }
void ACaseGameMode::Notify(const FString& Text) { Message=Text; MessageTime=10; }
AActor* ACaseGameMode::Target() const {
    auto* P=Cast<ACasePawn>(UGameplayStatics::GetPlayerPawn(this,0)); if(!P) return nullptr;
    FHitResult Hit; FVector From=P->Camera->GetComponentLocation(); FCollisionQueryParams Params; Params.AddIgnoredActor(P);
    GetWorld()->LineTraceSingleByChannel(Hit,From,From+P->Camera->GetForwardVector()*330,ECC_Visibility,Params);
    return Hit.GetActor();
}
void ACaseGameMode::Action(int32 Kind) {
    if(Blocked()) return;
    AActor* A=Target(); if(!A) {Notify(TEXT("Aproxime-se e aponte para um objeto identificado."));return;}
    Noise=5;
    if(A->ActorHasTag("Exit")) {Case.Leave();return;}
    if(A->ActorHasTag("Ritual")) {Notify(Case.DocumentRecovered?TEXT("Selo: 1 Nome / 2 Data / 3 Silêncio. Para abrir o limiar em vez de contê-lo: quadro, opção 3."):TEXT("O selo precisa do nome registrado no documento do cofre."));return;}
    const FName EvidenceTags[]={"Registry","Photo","Recording","Testimony","Report"};
    for(unsigned I=0;I<5;++I) if(A->ActorHasTag(EvidenceTags[I])) {
        if((I==1&&Kind!=1)||(I==2&&Kind!=2)||((I!=1&&I!=2)&&Kind!=0)) {Notify(I==1?TEXT("Use C para registrar a fotografia."):I==2?TEXT("Use G para ouvir e registrar a fita."):TEXT("Use E para examinar."));return;}
        auto R=Case.Collect(static_cast<Evidence>(I));
        if(R==Result::MissingEvidence) Notify(TEXT("Cofre lacrado: formule uma conclusão no quadro antes de recuperar o documento."));
        else Notify(EvidenceNames[I]);
        return;
    }
    Notify(TEXT("Sem evidência utilizável neste objeto."));
}
void ACaseGameMode::Choose(int32 N) {
    if(bPaused||Case.Outcome!=Ending::None) return;
    if(bBoard) {
        if(N==3) { auto R=Case.Release(); Notify(R==Result::Accepted?TEXT("Você abriu o limiar."):TEXT("Recupere o documento antes de decidir o destino da casa.")); return; }
        if(!Case.TestimonyChallenged) {
            auto R=Case.ChallengeWitness(); Notify(R==Result::Accepted?TEXT("Interfone: 'Não nascemos naquela época. A casa nos levou até lá.' A contradição foi confirmada."):TEXT("Reúna o registro, fotografe a família e ouça o depoimento antes de confrontá-lo.")); return;
        }
        auto R=Case.Conclude(N==1?Hypothesis::TemporalLoop:Hypothesis::Fraud);
        Notify(R==Result::Accepted?TEXT("Hipótese sustentada: a casa desloca seus ocupantes no tempo. O cofre DOI abriu."):R==Result::WrongConclusion?TEXT("Fraude não explica o registro vazio e a fita. Medo +10; reavalie as pistas."):R==Result::Duplicate?TEXT("Conclusão registrada. Recupere o documento no cofre."):TEXT("Ouça a fita antes de concluir."));
    } else if(AActor* A=Target(); A&&A->ActorHasTag("Ritual")) {
        auto R=Case.Ritual(N); Notify(R==Result::WrongSequence?TEXT("Ordem incorreta. Selo reiniciado; medo +15. Consulte a fita."):R==Result::MissingEvidence?TEXT("Recupere o relatório no cofre."):TEXT("Símbolo firmado."));
    }
}
FString ACaseGameMode::Objective() const {
    if(Case.DocumentRecovered) return TEXT("Decida: contenha no selo, abra o limiar pelo quadro ou saia com o documento.");
    if(Case.Conclusion) return TEXT("Recupere o relatório no cofre da sala leste.");
    if(Case.TestimonyChallenged) return TEXT("Ouça a fita e formule uma hipótese no quadro.");
    return TEXT("Reúna registro, fotografia e depoimento. Confronte a contradição no quadro.");
}
void ACaseGameMode::Tick(float Dt) {
    Super::Tick(Dt); if(Blocked()) return;
    auto* P=Cast<ACasePawn>(UGameplayStatics::GetPlayerPawn(this,0)); if(!P) return;
    Elapsed+=Dt; MessageTime=FMath::Max(0.f,MessageTime-Dt); Noise=FMath::Max(0.f,Noise-Dt);
    FVector Pos=P->GetActorLocation(); bool Safe=Pos.X < -850 && Pos.Y < 0;
    AActor* A=Target(); Focus=(A&&A->Tags.Num()>0)?TEXT("Objeto de investigação ao alcance"):TEXT("");
    bool Active=Elapsed>30&&Case.Count()>0;
    Entity->SetActorHiddenInGame(!Active);
    bool Threat=false;
    if(Active) {
        FHitResult H; FCollisionQueryParams Q; Q.AddIgnoredActor(Entity); Q.AddIgnoredActor(P);
        FVector E=Entity->GetActorLocation();
        bool Visible=!GetWorld()->LineTraceSingleByChannel(H,E,Pos,ECC_Visibility,Q);
        bool Heard=Noise>0 || FVector::Dist2D(Pos,LastPlayer)>1;
        if(!Safe&&((Visible&&FVector::Dist2D(E,Pos)<950)||(Heard&&FVector::Dist2D(E,Pos)<1100))) Investigate=Pos;
        if(Investigate.IsNearlyZero()||FVector::Dist2D(E,Investigate)<60) Investigate=FVector(FMath::Sin(Elapsed*.12f)*1000,160,130);
        FVector Waypoint=Investigate;
        const auto RoomCenter=[](float X) { return X < -550 ? -1100.f : (X > 550 ? 1100.f : 0.f); };
        if(E.Y>440 && (Investigate.Y<400 || RoomCenter(E.X)!=RoomCenter(Investigate.X))) {
            Waypoint=FVector(RoomCenter(E.X),E.Y,130);
            if(FMath::Abs(E.X-Waypoint.X)<55) Waypoint.Y=160;
        } else if(E.Y<440 && Investigate.Y>440) {
            Waypoint=FVector(RoomCenter(Investigate.X),160,130);
            if(FMath::Abs(E.X-Waypoint.X)<55) Waypoint=FVector(Waypoint.X,Investigate.Y,130);
        }
        FVector Direction=(Waypoint-E).GetSafeNormal2D(); FVector Next=E+Direction*Dt*95; Next.Z=130;
        // A swept visibility trace prevents the proxy entity from moving through walls.
        if(!GetWorld()->LineTraceSingleByChannel(H,E,Next+Direction*45,ECC_Visibility,Q)) Entity->SetActorLocation(Next);
        Threat=!Safe&&Visible&&FVector::Dist2D(Entity->GetActorLocation(),Pos)<360;
        SoundTime-=Dt;
        if(Knock&&Visible&&FVector::Dist2D(E,Pos)<1000&&SoundTime<=0) {UGameplayStatics::PlaySoundAtLocation(this,Knock,E,.55f,1.f,0.f,WarningAttenuation);SoundTime=2.5f;}
    }
    Case.Tick(Dt,Threat,Safe);
    P->Light->SetVisibility(Case.Flashlight);
    P->Light->SetIntensity(!bGentle&&Threat?14000+4000*FMath::Sin(Elapsed*3):18000);
    // A visible shelf only changes after it leaves the view; no passage is sealed.
    FVector ToShelf=(MovingShelf->GetActorLocation()-P->Camera->GetComponentLocation()).GetSafeNormal();
    if(!bLayoutChanged&&Case.Has(Evidence::Photo)&&FVector::DotProduct(P->Camera->GetForwardVector(),ToShelf)<-.25f) {
        MovingShelf->SetActorLocation(FVector(1450,650,100)); bLayoutChanged=true;
        Notify(TEXT("Um móvel arrasta atrás de você. O corredor de saída permanece acessível."));
    }
    LastPlayer=Pos;
}
void ACaseHUD::DrawHUD() {
    Super::DrawHUD(); auto* G=GM(this); if(!G||!Canvas||!GEngine) return;
    const float S=FMath::Clamp(Canvas->ClipY/850.f,.55f,1.7f), W=Canvas->ClipX;
    auto Text=[&](const FString& T,float X,float Y,FLinearColor C=FLinearColor::White) {DrawText(T,C,X,Y,GEngine->GetSmallFont(),S*1.15f);};
    DrawRect(FLinearColor(0,0,0,.8),0,0,W,112*S);
    Text(TEXT("DOI / ARQUIVOS DO OUTRO LADO / CASO 001"),20*S,14*S,FLinearColor(.85,.72,.45));
    Text(FString::Printf(TEXT("Medo %.0f / 100    Bateria %.0f%%    Evidências %u/5    F8: %s"),G->Case.Fear,G->Case.Battery,G->Case.Count(),G->bGentle?TEXT("suave"):TEXT("intensa")),20*S,40*S);
    Text(G->Objective(),20*S,66*S);
    Text(TEXT("WASD mover | Mouse olhar | E examinar | C foto | G gravador | F lanterna | TAB quadro | ESC pausa"),20*S,89*S);
    if(G->bBoard) {
        DrawRect(FLinearColor(.025,.03,.035,.98),0,112*S,W,Canvas->ClipY-112*S);
        float Y=140*S; Text(TEXT("QUADRO / A CASA SEM REGISTRO — tempo suspenso"),20*S,Y);Y+=36*S;
        for(unsigned I=0;I<5;++I) {Text(G->Case.Has(static_cast<Evidence>(I))?EvidenceNames[I]:TEXT("[ Evidência ainda não registrada ]"),20*S,Y);Y+=32*S;}
        Y+=20*S;
        Text(G->Case.TestimonyChallenged?TEXT("Vínculo confirmado: o depoimento contradiz a fotografia e o registro."):TEXT("1 ou 2: confrontar depoimento usando registro + foto."),20*S,Y);Y+=36*S;
        Text(TEXT("Após confrontar e ouvir a fita: 1 deslocamento temporal / 2 fraude."),20*S,Y);Y+=36*S;
        Text(TEXT("3 abrir o limiar (irreversível, exige documento) / TAB voltar"),20*S,Y);
    } else {
        DrawRect(FLinearColor(.8,.8,.7,.9),W*.5f-2,Canvas->ClipY*.5f-2,4,4);
        Text(G->Focus,20*S,Canvas->ClipY-110*S);
    }
    if(G->MessageTime>0) {DrawRect(FLinearColor(0,0,0,.9),0,Canvas->ClipY-75*S,W,75*S);Text(G->Message,20*S,Canvas->ClipY-55*S);}
    if(G->bPaused) {DrawRect(FLinearColor(0,0,0,.9),0,120*S,W,100*S);Text(TEXT("PAUSADO / ESC continuar"),20*S,150*S);}
    if(G->Case.Exposure>0&&!G->bBoard) Text(TEXT("PRESENÇA PRÓXIMA — afaste-se ou volte ao abrigo iluminado."),20*S,125*S,FLinearColor(1,.4,.25));
    if(G->Case.Outcome!=Ending::None) {
        DrawRect(FLinearColor(0,0,0,.97),0,112*S,W,Canvas->ClipY-112*S);
        const TCHAR* End=G->Case.Outcome==Ending::Contained?TEXT("CONTENÇÃO: a família retorna. O DOI exige que você destrua o relatório."):
            G->Case.Outcome==Ending::Released?TEXT("LIMIAR ABERTO: a família sai. Outras vozes atravessam com ela."):
            G->Case.Outcome==Ending::Lost?TEXT("DESAPARECIDO: agora você também está na fotografia de 1978."):
            G->Case.DocumentRecovered?TEXT("EXTRAÇÃO: você entrega o relatório. O DOI já conhecia seu nome."):TEXT("RETIRADA: você saiu sem o documento. A casa continua esperando.");
        Text(End,20*S,170*S); Text(TEXT("R reiniciar o caso"),20*S,215*S);
    }
}
