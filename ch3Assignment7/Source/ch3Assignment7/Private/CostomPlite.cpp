// Fill out your copyright notice in the Description page of Project Settings.


#include "CostomPlite.h"
#include "Components/BoxComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"

// Sets default values
ACostomPlite::ACostomPlite()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BoxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("CapsuleComp"));
	BoxComp->SetBoxExtent(FVector(150.f, 150.f, 96.f));
	RootComponent = BoxComp;
	BoxComp->SetSimulatePhysics(false);

	SkeletalMeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMeshComp"));
	SkeletalMeshComp->SetupAttachment(RootComponent);
	SkeletalMeshComp->SetSimulatePhysics(false);

	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComp"));
	SpringArmComp->SetupAttachment(RootComponent);
	SpringArmComp->TargetArmLength = 500.f;
	//SpringArmComp->bUsePawnControlRotation = true;

	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
	CameraComp->SetupAttachment(SpringArmComp, USpringArmComponent::SocketName);
	//CameraComp->bUsePawnControlRotation = false;
}

// Called when the game starts or when spawned
void ACostomPlite::BeginPlay()
{
	Super::BeginPlay();

	APlayerController* PC = Cast<APlayerController>(GetController());
	if (IsValid(PC)) {
		ULocalPlayer* Player = PC->GetLocalPlayer();
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(Player)) {
			Subsystem->AddMappingContext(InputMappingContext, 0);
		}
	}
	
}

// Called every frame
void ACostomPlite::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ACostomPlite::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ACostomPlite::OnMoveTriggered);
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Completed, this, &ACostomPlite::OnMoveCompleted);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ACostomPlite::Look);
	}
}

void ACostomPlite::OnMoveTriggered(const FInputActionValue& Value)
{
	FVector MovementVector = Value.Get<FVector>();

	if (Controller != nullptr)
	{
		FRotator CamRot = GetActorRotation();
		FRotator YawRot(0.f, CamRot.Yaw, 0.f);

		//const FVector Forward = FRotationMatrix(YawRot).GetUnitAxis(EAxis::X);
		const FVector Forward = GetActorForwardVector();
		const FVector Right = FRotationMatrix(YawRot).GetUnitAxis(EAxis::Y);

		float MoveSpeed = 500.f;
		//FVector MoveDelta = (Forward * MovementVector.Y + Right * MovementVector.X)	* MoveSpeed * GetWorld()->GetDeltaSeconds();

		FVector HorizontalMove = (Forward * MovementVector.X + Right * MovementVector.Y);

		// Space/Shift (Z) 이동 – 로컬Z축 기준
		FVector VerticalMove = FVector::UpVector * MovementVector.Z;

		FVector MoveDelta = (HorizontalMove + VerticalMove) * MoveSpeed * GetWorld()->GetDeltaSeconds();
		AddActorWorldOffset(MoveDelta, true);
	}
}

void ACostomPlite::OnMoveCompleted(const FInputActionValue& Value)
{
}


void ACostomPlite::Look(const FInputActionValue& Value)
{
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		float TurnRate = 45.f;
		float PitchRate = 45.f;

		float YawInput = LookAxisVector.X * TurnRate * GetWorld()->GetDeltaSeconds();
		float PitchInput = LookAxisVector.Y * PitchRate * GetWorld()->GetDeltaSeconds();

		FRotator NewRotation = GetActorRotation();
		NewRotation.Yaw += YawInput;
		NewRotation.Pitch = FMath::Clamp(NewRotation.Pitch + PitchInput, -80.f, 80.f);

		SetActorRotation(NewRotation);
		
	}

}
