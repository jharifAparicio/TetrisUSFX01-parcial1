// Fill out your copyright notice in the Description page of Project Settings.


#include "Piece.h"
#include "Components/SceneComponent.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include <vector>
#include "Sound/SoundCue.h"


// carga los valores inicales de la pieza
APiece::APiece() {
	//Configura este actor para llamar a la funci�n Tick() en cada cuadro.
	//Puedes desactivarlo para mejorar el rendimiento si no es necesario.
	PrimaryActorTick.bCanEverTick = true;

	SceneComponent = CreateDefaultSubobject<USceneComponent>("Pieces Scene");
	RootComponent = SceneComponent;

	struct FConstructorStatics
	{
		ConstructorHelpers::FObjectFinderOptional<UMaterial> Color_0;
		ConstructorHelpers::FObjectFinderOptional<UMaterial> Color_1;
		ConstructorHelpers::FObjectFinderOptional<UMaterial> Color_2;
		ConstructorHelpers::FObjectFinderOptional<UMaterial> Color_3;
		ConstructorHelpers::FObjectFinderOptional<UMaterial> Color_4;
		ConstructorHelpers::FObjectFinderOptional<UMaterial> Color_5;
		ConstructorHelpers::FObjectFinderOptional<UMaterial> Color_6;
		ConstructorHelpers::FObjectFinderOptional<UMaterial> Color_7;

		FConstructorStatics() :
			Color_0(TEXT("Material'/Game/Mesh/Material_0.Material_0'")),
			Color_1(TEXT("Material'/Game/Mesh/Material_1.Material_1'")),
			Color_2(TEXT("Material'/Game/Mesh/Material_2.Material_2'")),
			Color_3(TEXT("Material'/Game/Mesh/Material_3.Material_3'")),
			Color_4(TEXT("Material'/Game/Mesh/Material_4.Material_4'")),
			Color_5(TEXT("Material'/Game/Mesh/Material_5.Material_5'")),
			Color_6(TEXT("Material'/Game/Mesh/Material_6.Material_6'")),
			Color_7(TEXT("Material'/Game/Mesh/Material_006.Material_006'"))
		{
		}
	};

	static FConstructorStatics ConstructorStatics;
	Colors.Add(ConstructorStatics.Color_0.Get());
	Colors.Add(ConstructorStatics.Color_1.Get());
	Colors.Add(ConstructorStatics.Color_2.Get());
	Colors.Add(ConstructorStatics.Color_3.Get());
	Colors.Add(ConstructorStatics.Color_4.Get());
	Colors.Add(ConstructorStatics.Color_5.Get());
	Colors.Add(ConstructorStatics.Color_6.Get());
	Colors.Add(ConstructorStatics.Color_7.Get());

	static ConstructorHelpers::FObjectFinder<USoundCue> RotateSoundCueObject(TEXT("SoundCue'/Game/Sounds/block-rotate_Cue.block-rotate_Cue'"));
	if (RotateSoundCueObject.Succeeded()) {
		RotateSoundCue = RotateSoundCueObject.Object;
	}

	static ConstructorHelpers::FObjectFinder<USoundCue> MoveSoundCueObject(TEXT("SoundCue'/Game/Sounds/slow-hit_Cue.slow-hit_Cue'"));
	if (MoveSoundCueObject.Succeeded()) {
		MoveLeftRightSoundCue = MoveSoundCueObject.Object;
	}
}

// Called when the game starts or when spawned
void APiece::BeginPlay() {
	Super::BeginPlay();

	SpawnPieces();
}

// Called every frame
void APiece::Tick(float DeltaTime) {

	Super::Tick(DeltaTime);
}

void APiece::SpawnPieces() {
	vector<vector<pair<float, float>>> Shapes = {
		{{-20.0f, 0.0f}, {-10.0f, 0.0f}, {0.0f, 0.0f}, {10.0f, 0.0f}},
		{{0.0f, 10.0f}, {0.0, 0.0}, {10.0, 0.0}, {20.0, 0.0}},
		{{-20.0, 0.0}, {-10.0, 0.0}, {0.0, 0.0}, {0.0, 10.0}},
		{{0.0, 0.0}, {10.0, 0.0}, {0.0, -10.0}, {10.0, -10.0}},
		{{-10.0, -10.0}, {0.0, -10.0}, {0.0, 0.0}, {10.0, 0.0}},
		{{-10.0, 0.0}, {0.0, 0.0}, {0.0, 10.0}, {10.0, 0.0}},
		{{-10.0, 0.0}, {0.0, 0.0}, {0.0, -10.0}, {10.0, -10.0}},
		{{-20.0, 10.0}, {-10.0, 0.0}, {0.0, 10.0}, {10.0, 0.0}}
	};

	const int Index = FMath::RandRange(0, Shapes.size() - 1);

	UE_LOG(LogTemp, Warning, TEXT("index=%d"), Index);

	const vector<pair<float, float>>& YZs = Shapes[Index];
	const int ColorIndex = FMath::RandRange(0, Shapes.size() - 1);

	for (auto&& YZ : YZs) {
		FRotator Rotation(0.0, 0.0, 0.0);
		ABlock* B = GetWorld()->SpawnActor<ABlock>(this->GetActorLocation(), Rotation);
		B->BlockMesh->SetMaterial(1, Colors[ColorIndex]);
		Blocks.Add(B);
		B->AttachToActor(this, FAttachmentTransformRules::KeepRelativeTransform);
		B->SetActorRelativeLocation(FVector(0.0, YZ.first, YZ.second));
	}

}

void APiece::EndPlay(const EEndPlayReason::Type EndPlayReason) {
	UE_LOG(LogTemp, Warning, TEXT("Piezas eliminadas"));
}

void APiece::DrawDebugPiece() {
	for (ABlock* B : Blocks) {
		FVector V1 = B->GetActorLocation();
		FVector V2(V1.X, V1.Y, 5.0f);
		DrawDebugLine(GetWorld(), V1, V2, FColor::Green, false, 1, 0, 1);
	}
}

bool APiece::CheckWillCollision(function<FVector(FVector OldLocation)> ChangeBeforeCheck) {
	FCollisionQueryParams Params;

	for (ABlock* B : Blocks) {
		Params.AddIgnoredActor(B);
	}

	for (ABlock* B : Blocks) {
		FVector TempVector = B->GetActorLocation();
		TempVector = ChangeBeforeCheck(TempVector);

		TArray<struct FOverlapResult> OutOverlaps;
		FCollisionShape CollisionShapes;
		CollisionShapes.SetBox(FVector(4.0f, 4.0f, 4.0f));
		FCollisionResponseParams ResponseParam;
		bool b = GetWorld()->OverlapMultiByChannel(OutOverlaps,
			TempVector, B->GetActorQuat(), ECollisionChannel::ECC_WorldDynamic,
			CollisionShapes, Params, ResponseParam);

		if (b) {
			for (auto&& Result : OutOverlaps) {
				UE_LOG(LogTemp, Warning, TEXT("OverLapped with actor %s, component=%s"),
					   *Result.GetActor()->GetName(),
					*Result.GetComponent()->GetName());
			}
			return true;
		}
	}
	
	return false;
}

void APiece::TestRotate() {
	//funcion lambda
	auto RotateVector = [this](FVector OldVector) {
		FVector ActorVector = this->GetActorLocation();
		FVector BlockVector = OldVector;
		FVector TempVector = BlockVector - ActorVector;
		TempVector = ActorVector + TempVector.RotateAngleAxis(90.0, FVector(1.0, 0.0, 0.0));
		return TempVector;
	};

	if (!CheckWillCollision(RotateVector)) {
		UE_LOG(LogTemp, Warning, TEXT("now can rotate"));
		FRotator NewRotation = this->GetActorRotation() + FRotator(0.0, 0.0, -90.0);
		this->SetActorRelativeRotation(NewRotation);
		if (RotateSoundCue) {
			UGameplayStatics::PlaySoundAtLocation(GetWorld(), RotateSoundCue, GetActorLocation(), GetActorRotation());
		}
	}
}

void APiece::MoveLeft() {
	auto MoveVectorLeft = [](FVector OldVector) {
		OldVector.Y -= 10.0f;
		return OldVector;
	};

	if (!CheckWillCollision(MoveVectorLeft)) {
		FVector NewLocation = GetActorLocation();
		NewLocation.Y -= 10;
		SetActorLocation(NewLocation);

		if (MoveLeftRightSoundCue) {
			UGameplayStatics::PlaySoundAtLocation(GetWorld(), MoveLeftRightSoundCue, GetActorLocation(), GetActorRotation());
		}
	}
}

void APiece::MoveRight() {
	auto MoveVectorRight = [](FVector OldVector) {
		OldVector.Y += 10.0f;
		return OldVector;
	};

	if (!CheckWillCollision(MoveVectorRight)) {
		FVector NewLocation = GetActorLocation();
		NewLocation.Y += 10;
		SetActorLocation(NewLocation);

		if (MoveLeftRightSoundCue) {
			UGameplayStatics::PlaySoundAtLocation(GetWorld(), MoveLeftRightSoundCue, GetActorLocation(), GetActorRotation());
		}
	}
}

bool APiece::MoveDown(bool PlaySound) {


	auto MoveVectorDown = [=] (FVector OldVector) {
		OldVector.Z -= 10.0f;
		OldVector.Y += directionP;

		return OldVector;
	};

	if (!CheckWillCollision (MoveVectorDown)) {
		FVector NewLocation = GetActorLocation ();
		NewLocation.Z -= 10;
		NewLocation.Y += directionP;
		SetActorLocation (NewLocation);

		return true;
	} else {
		return false;
	}	
}

void APiece::Dismiss() {
	Blocks.Empty();
}
