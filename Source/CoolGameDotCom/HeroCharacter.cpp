#include "HeroCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

AHeroCharacter::AHeroCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	// Запрещаем повороту контроллера вращать самого персонажа
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Настройка движения: отключаем авторазворот по направлению ходьбы
	GetCharacterMovement()->bOrientRotationToMovement = false;
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;
	GetCharacterMovement()->MaxWalkSpeed = 650.0f;

	// Создаем стрелу камеры (SpringArm)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->SetUsingAbsoluteRotation(true); // Камера не крутится вместе с персонажем
	CameraBoom->TargetArmLength = 1200.0f;
	CameraBoom->SetRelativeRotation(FRotator(-55.0f, 0.0f, 0.0f)); // Классический угол Hades
	CameraBoom->bDoCollisionTest = false; // Камера не дергается при коллизии со стенами

	// Создаем камеру
	TopDownCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("TopDownCamera"));
	TopDownCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	TopDownCamera->bUsePawnControlRotation = false;

	// Начальные параметры рывка
	DashDistance = 2500.0f;
	DashCooldown = 0.8f;
	InvulnerabilityDuration = 0.25f;
	bCanDash = true;
	bIsInvulnerable = false;
}

void AHeroCharacter::BeginPlay()
{
	Super::BeginPlay();

	// Включаем отображение курсора мыши
	APlayerController* PC = Cast<APlayerController>(GetController());
	if (PC)
	{
		PC->bShowMouseCursor = true;
		PC->bEnableClickEvents = true;
		PC->bEnableMouseOverEvents = true;
	}
}

void AHeroCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Каждый кадр поворачиваем корпус персонажа лицом к курсору
	RotateTowardsCursor();
}

void AHeroCharacter::RotateTowardsCursor()
{
	APlayerController* PC = Cast<APlayerController>(GetController());
	if (!PC) return;

	FHitResult HitResult;
	// Пускаем луч от камеры через курсор мыши в плоскость мира
	if (PC->GetHitResultUnderCursorByChannel(UEngineTypes::ConvertToTraceType(ECC_Visibility), true, HitResult))
	{
		FVector TargetLocation = HitResult.Location;
		TargetLocation.Z = GetActorLocation().Z; // Игнорируем высоту, чтобы персонаж не наклонялся в землю

		// Находим угол поворота к точке курсора
		FRotator TargetRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), TargetLocation);

		// Плавный поворот (интерполяция)
		FRotator NewRotation = FMath::RInterpTo(GetActorRotation(), TargetRotation, GetWorld()->GetDeltaSeconds(), 20.0f);
		SetActorRotation(NewRotation);
	}
}

void AHeroCharacter::PerformDash()
{
	// Проверяем кулдаун
	if (!bCanDash) return;

	// Определяем направление рывка:
	// Если игрок зажимает WASD — берем вектор ввода, если стоит на месте — направление взгляда
	FVector DashDirection = GetLastMovementInputVector();
	if (DashDirection.IsNearlyZero())
	{
		DashDirection = GetActorForwardVector();
	}
	DashDirection.Z = 0.0f;
	DashDirection.Normalize();

	// Применяем резкий импульс движения
	LaunchCharacter(DashDirection * DashDistance, true, true);

	// Блокируем дэш и включаем окно неуязвимости
	bCanDash = false;
	bIsInvulnerable = true;

	// Вызываем событие для Blueprint (партиклы, звук)
	OnDashExecuted();

	GEngine->AddOnScreenDebugMessage(-1, 0.5f, FColor::Cyan, TEXT("DASH! (Неуязвимость ВКЛ)"));

	// Таймер 1: выключение неуязвимости
	GetWorldTimerManager().SetTimer(InvulnerabilityTimerHandle, this, &AHeroCharacter::EndInvulnerability, InvulnerabilityDuration, false);

	// Таймер 2: откат кулдауна рывка
	GetWorldTimerManager().SetTimer(CooldownTimerHandle, this, &AHeroCharacter::ResetDash, DashCooldown, false);
}

void AHeroCharacter::EndInvulnerability()
{
	bIsInvulnerable = false;
	GEngine->AddOnScreenDebugMessage(-1, 0.5f, FColor::Silver, TEXT("Неуязвимость ВЫКЛ"));
}

void AHeroCharacter::ResetDash()
{
	bCanDash = true;
	GEngine->AddOnScreenDebugMessage(-1, 0.8f, FColor::Green, TEXT("Дэш готов!"));
}