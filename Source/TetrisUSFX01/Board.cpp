// Fill out your copyright notice in the Description page of Project Settings.


#include "Board.h"
#include "EngineUtils.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
// Sets default values

ABoard::ABoard() {
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	static ConstructorHelpers::FObjectFinder<USoundCue> LineRemove_Sound(TEXT("SoundCue'/Game/Sounds/line-remove_Cue.line-remove_Cue'"));
	if (LineRemove_Sound.Succeeded()) {
		LineRemoveSoundCue = LineRemove_Sound.Object;
	}

	static ConstructorHelpers::FObjectFinder<USoundCue> MoveToEnd_Sound(TEXT("SoundCue'/Game/Sounds/force-hit_Cue.force-hit_Cue'"));
	if (MoveToEnd_Sound.Succeeded()) {
		MoveToEndSoundCue = MoveToEnd_Sound.Object;
	}

	static ConstructorHelpers::FObjectFinder<USoundCue> GameStart_Sound(TEXT("SoundCue'/Game/Sounds/start_Cue.start_Cue'"));
	if (GameStart_Sound.Succeeded()) {
		GameStartSoundCue = GameStart_Sound.Object;
	}

	static ConstructorHelpers::FObjectFinder<USoundCue> GameOver_Sound(TEXT("SoundCue'/Game/Sounds/gameover_Cue.gameover_Cue'"));
	if (GameOver_Sound.Succeeded()) {
		GameOverSoundCue = GameOver_Sound.Object;
	}

	directionBoard = 0;
}

// Called when the game starts or when spawned
void ABoard::BeginPlay() {
	Super::BeginPlay();

	for (TActorIterator<APiece> it(GetWorld()); it; ++it) {
		if (it->GetFName() == TEXT("DissmissPieces")) {
			it->Dismiss();
			it->Destroy();
		}
	}
}

// Called every frame
void ABoard::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	if (bGameOver) {
		return;
	}

	switch (Status) {
		case PS_NOT_INITED:
			NewPiece();
			CoolLeft = CoolDown;
			Status = PS_MOVING;
			break;
		case PS_MOVING:
			CoolLeft -= DeltaTime;
			if (CoolLeft <= 0.0f) {
				MoveDown();
			}
			break;
		case PS_GOT_BOTTOM:
			CoolLeft -= DeltaTime;
			if (CoolLeft <= 0.0f) {
				if (CurrentPiece) {
					CurrentPiece->Dismiss();
					CurrentPiece->Destroy();
				}
				CurrentPiece = nullptr;
				NewPiece();
				CoolLeft = CoolDown;
				Status = PS_MOVING;
				if (directionBoard == -10) {
					directionBoard = 0;
				} else if (directionBoard == 0) {
					directionBoard = 10;
				} else if (directionBoard == 10) {
					directionBoard = -10;
				}
				CurrentPiece->directionP = directionBoard;
			}
			break;
		default:
			break;
	}
}

// Called to bind functionality to input
void ABoard::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) {
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction("Rotate", IE_Pressed, this, &ABoard::Rotate);

	PlayerInputComponent->BindAction("RotateFB", IE_Pressed, this, &ABoard::RotateFB);
	
	PlayerInputComponent->BindAction("MoveLeft", IE_Pressed, this, &ABoard::MoveLeft);
	PlayerInputComponent->BindAction("MoveRight", IE_Pressed, this, &ABoard::MoveRight);
	PlayerInputComponent->BindAction("MoveDown", IE_Pressed, this, &ABoard::MoveDown);
	PlayerInputComponent->BindAction("MoveDownToEnd", IE_Pressed, this, &ABoard::MoveDownToEnd);
	//PlayerInputComponent->BindAction("NewPiece", IE_Pressed, this, &ABoard::NewPiece);
	//PlayerInputComponent->BindAction("CheckLine", IE_Pressed, this, &ABoard::CheckLine);

}

void ABoard::Rotate() {
	if (CurrentPiece && Status != PS_GOT_BOTTOM) {
		CurrentPiece->TestRotate();
	}
}

void ABoard::RotateFB() {
	if (CurrentPiece && Status != PS_GOT_BOTTOM) {
		CurrentPiece->TestRotateFB();
	}
}

void ABoard::MoveLeft() {
	if (CurrentPiece) {
		CurrentPiece->MoveLeft();
		if (Status == PS_GOT_BOTTOM) {
			MoveDownToEnd();
		}
	}
}

void ABoard::MoveRight() {
	if (CurrentPiece) {
		CurrentPiece->MoveRight();
		if (Status == PS_GOT_BOTTOM) {
			MoveDownToEnd();
		}
	}
}

void ABoard::MoveDown() {
	if (CurrentPiece) {
		if (!CurrentPiece->MoveDown()) {
			Status = PS_GOT_BOTTOM;
		}
		CoolLeft = CoolDown;
	}
}

bool ABoard::CheckGameOver() {
	return CurrentPiece->CheckWillCollision([](FVector OldVector) {
		return OldVector;
	}
	);

	if (!CurrentPiece) {
		UE_LOG(LogTemp, Warning, TEXT("NoPieces"));
		return true;
	}
}

void ABoard::NewPiece () {
	CheckLine();
	if (CurrentPiece) {
		CurrentPiece->Dismiss();
		CurrentPiece->Destroy();
	}
	FVector Location (0.0, 5.0, 195.0);

	FRotator Rotation(0.0, 0.0, 0.0);
	CurrentPiece = GetWorld()->SpawnActor<APiece>(Location, Rotation);

	bGameOver = CheckGameOver();
	if (bGameOver) {
		UE_LOG(LogTemp, Warning, TEXT("Game Over!!!!!!!!"));
		GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Magenta, TEXT("Game Over"));
		if (GameOverSoundCue)
		{
			UGameplayStatics::PlaySoundAtLocation(GetWorld(), GameOverSoundCue, GetActorLocation(), GetActorRotation());
		}
	}
}

void ABoard::CheckLine() {
	//funcion lambda
	auto MoveDownFromLine = [this](int z) {
		FVector Location(0.0f, 0.0f, 5.0 * z + 100.0);
		FRotator Rotation(0.0f, 0.0f, 0.0f);
		TArray<struct FOverlapResult> OutOverlaps;
		FCollisionShape CollisionShape;
		FVector Extent(4.5f, 49.5f, 95.0 + 4.5 - 5.0 * z);
		CollisionShape.SetBox(Extent);
		//DrawDebugBox(GetWorld(), Location, Extent, FColor::Purple, true, 1, 1, 1);
		FCollisionQueryParams Params;
		FCollisionResponseParams ResponseParam;
		if (GetWorld()->OverlapMultiByChannel(OutOverlaps,
											  Location, Rotation.Quaternion(), ECollisionChannel::ECC_WorldDynamic,
											  CollisionShape, Params, ResponseParam)) {
			for (auto&& Result : OutOverlaps) {
				FVector NewLocation = Result.GetActor()->GetActorLocation();
				NewLocation.Z -= 10.0f;
				Result.GetActor()->SetActorLocation(NewLocation);
			}
		}
	};

	int z = 0;
	while (z < 20) {
		FVector Location(0.0f, 0.0f, 10.0f * z + 5.0f);
		FRotator Rotation(0.0f, 0.0f, 0.0f);
		TArray<struct FOverlapResult> OutOverlaps;
		FCollisionShape CollisionShape;
		CollisionShape.SetBox(FVector(4.0f, 49.0f, 4.0f));
		//DrawDebugBox(GetWorld(), Location, FVector(4.5f, 49.5f, 4.5f), FColor::Purple, false, 1, 0, 1);
		FCollisionQueryParams Params;
		FCollisionResponseParams ResponseParam;
		bool b = GetWorld()->OverlapMultiByChannel(OutOverlaps,
			Location, Rotation.Quaternion(), ECollisionChannel::ECC_WorldDynamic,
			CollisionShape, Params, ResponseParam);
		if (!b || OutOverlaps.Num() < 10) {
			++z;
			continue;
		} else { // this line is full, remove the line
			UE_LOG(LogTemp, Warning, TEXT("Find FULL LINE at z=%d"), z);
			GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, TEXT("Find FULL LINE"));
			for (auto&& Result : OutOverlaps) {
				Result.GetActor()->Destroy();
			}
			MoveDownFromLine(z);

			if (LineRemoveSoundCue)
			{
				UGameplayStatics::PlaySoundAtLocation(GetWorld(), LineRemoveSoundCue, GetActorLocation(), GetActorRotation());
			}
		}
	}
}

void ABoard::MoveDownToEnd() {
	if (!CurrentPiece) {
		return;
	}

	while (CurrentPiece->MoveDown()) {

	}
	UE_LOG(LogTemp, Warning, TEXT("se fue al final"));

	if (MoveToEndSoundCue)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), MoveToEndSoundCue, GetActorLocation(), GetActorRotation());
	}

	switch (Status) {
		case PS_MOVING:
			Status = PS_GOT_BOTTOM;
			CoolLeft = CoolDown;
			break;
		case PS_GOT_BOTTOM:
			break;
		default:
			UE_LOG(LogTemp, Warning, TEXT("Wrong status for MoveDownToEnd"));
			GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Blue, TEXT("Wrong status for MoveDownToEnd"));
			break;
	}
}