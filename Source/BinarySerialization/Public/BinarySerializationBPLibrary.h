// Copyright WOW Inc. All Rights Reserved.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "BinarySerializationBPLibrary.generated.h"

UCLASS()
class UBinarySerializationBPLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()

	const static int32 vectorDataSize = sizeof(float) * 3;
	const static int32 quatDataSize = sizeof(double) * 4;
	const static int32 rotatorDataSize = sizeof(float) * 3;

	template <typename T> static void WriteValueToBytes(const T& value, TArray<uint8>& Dst, int DstStartIndex) {
		if (Dst.Num() - 1 < DstStartIndex) {
			return;
		}

		const uint8* ptr = reinterpret_cast<const uint8*>(&value);

		for (int i = 0; i < sizeof(T); i++) {
			int dstIndex = DstStartIndex + i;

			if (dstIndex < Dst.Num()) {
				Dst[dstIndex] = ptr[i];
			}
		}
	}

	template <typename T> static T ReadValueFromBytes(TArray<uint8>& Source, int SourceStartIndex) {
		return *(reinterpret_cast<const T*>(Source.GetData() + SourceStartIndex));
	}

	UFUNCTION(BlueprintCallable, Category = "BinarySerialization")
	static void WriteIntToBytes(const int32& value, UPARAM(ref) TArray<uint8>& Dst, int DstStartIndex) {
		WriteValueToBytes<int32>(value, Dst, DstStartIndex);
	}

	UFUNCTION(BlueprintCallable, Category = "BinarySerialization")
	static int32 ReadIntFromBytes(UPARAM(ref) TArray<uint8>& Source, int SourceStartIndex) {
		return ReadValueFromBytes<uint32>(Source, SourceStartIndex);
	}

	UFUNCTION(BlueprintCallable, Category = "BinarySerialization")
	static void WriteFloatToBytes(const float& value, UPARAM(ref) TArray<uint8>& Dst, int DstStartIndex) {
		WriteValueToBytes<float>(value, Dst, DstStartIndex);
	}

	UFUNCTION(BlueprintCallable, Category = "BinarySerialization")
	static float ReadFloatFromBytes(UPARAM(ref) TArray<uint8>& Source, int SourceStartIndex) {
		return ReadValueFromBytes<float>(Source, SourceStartIndex);
	}

	UFUNCTION(BlueprintCallable, Category = "BinarySerialization")
	static void WriteVectorToBytes(const FVector& value, UPARAM(ref) TArray<uint8>& Dst, int DstStartIndex) {
		if (Dst.Num() - 1 < DstStartIndex) {
			return;
		}

		for (int v = 0; v < 3; v++) {
			float element = value[v];
			const uint8* ptr = reinterpret_cast<const uint8*>(&element);

			for (int i = 0; i < sizeof(float); i++) {
				int dstIndex = v * sizeof(float) + DstStartIndex + i;

				if (dstIndex < Dst.Num()) {
					Dst[dstIndex] = ptr[i];
				}
			}
		}
	}

	UFUNCTION(BlueprintCallable, Category = "BinarySerialization")
	static FVector ReadBytesToVector(UPARAM(ref) TArray<uint8>& Source, int SourceStartIndex) {
		FVector vector;

		for (int dim = 0; dim < 3; dim++) {
			int index = SourceStartIndex + dim * sizeof(float);
			float element = *(reinterpret_cast<float*>(Source.GetData() + index));

			vector[dim] = element;
		}

		return vector;
	}

	UFUNCTION(BlueprintCallable, Category = "BinarySerialization")
	static void WriteQuatToBytes(const FQuat& value, UPARAM(ref) TArray<uint8>& Dst, int DstStartIndex) {
		if (Dst.Num() - 1 < DstStartIndex) {
			return;
		}

		WriteValueToBytes<double>(value.X, Dst, DstStartIndex);
		WriteValueToBytes<double>(value.Y, Dst, DstStartIndex + sizeof(double));
		WriteValueToBytes<double>(value.Z, Dst, DstStartIndex + sizeof(double) * 2);
		WriteValueToBytes<double>(value.W, Dst, DstStartIndex + sizeof(double) * 3);
	}

	UFUNCTION(BlueprintCallable, Category = "BinarySerialization")
	static FQuat ReadBytesToQuat(UPARAM(ref) TArray<uint8>& Source, int SourceStartIndex) {
		FQuat quat;

		int index = SourceStartIndex + 0 * sizeof(double);
		float x = *(reinterpret_cast<double*>(Source.GetData() + index));
		quat.X = x;

		index += sizeof(double);
		float y = *(reinterpret_cast<double*>(Source.GetData() + index));
		quat.Y = y;

		index += sizeof(double);
		float z = *(reinterpret_cast<double*>(Source.GetData() + index));
		quat.Z = z;

		index += sizeof(double);
		float w = *(reinterpret_cast<double*>(Source.GetData() + index));
		quat.W = w;

		return quat;
	}

	UFUNCTION(BlueprintCallable, Category = "BlueprintSerialization")
	static void WriteRotatorToBytes(const FRotator& rotator, UPARAM(ref) TArray<uint8>& Dst, int DstStartIndex) {
		int index = DstStartIndex;
		
		WriteValueToBytes<float>(rotator.Pitch, Dst, index);

		index += sizeof(float);
		WriteValueToBytes<float>(rotator.Roll, Dst, index);
		
		index += sizeof(float);
		WriteValueToBytes<float>(rotator.Yaw, Dst, index);
	}

	UFUNCTION(BlueprintCallable, Category = "BlueprintSerialization")
	static FRotator ReadRotatorFromBytes(UPARAM(ref) TArray<uint8>& Source, int SourceStartIndex) {
		FRotator rotator;
		int index = SourceStartIndex;

		rotator.Pitch = ReadValueFromBytes<float>(Source, index);
		
		index += sizeof(float);
		rotator.Roll = ReadValueFromBytes<float>(Source, index);
		
		index += sizeof(float);
		rotator.Yaw = ReadValueFromBytes<float>(Source, index);

		return rotator;
	}

	UFUNCTION(BlueprintPure, Category = "BinarySerialization")
	static int32 GetTransformByteLength() {
		return vectorDataSize + rotatorDataSize + vectorDataSize;
	}

	UFUNCTION(BlueprintPure, Category = "BinarySerialization")
	static int32 GetVector3ByteLength() {
		return vectorDataSize;
	}

	UFUNCTION(BlueprintPure, Category = "BinarySerialization")
	static int32 GetRotatorByteLength() {
		return rotatorDataSize;
	}

	UFUNCTION(BlueprintCallable, Category = "BinarySerialization")
	static void WriteTransformToBytes(const FTransform& transform, UPARAM(ref) TArray<uint8>& Dst, int DstStartIndex) {
		int index = DstStartIndex;
		
		WriteVectorToBytes(transform.GetLocation(), Dst, index);

		index += vectorDataSize;
		WriteRotatorToBytes(transform.GetRotation().Rotator(), Dst, index);

		index += rotatorDataSize;
		WriteVectorToBytes(transform.GetScale3D(), Dst, index);
	}

	UFUNCTION(BlueprintCallable, Category = "BinarySerialization")
	static FTransform ReadTransformToBytes(UPARAM(ref) TArray<uint8>& Source, int SourceStartIndex) {
		FTransform transform;
		int index = SourceStartIndex;
		transform.SetLocation(ReadBytesToVector(Source, index));

		index += vectorDataSize;
		transform.SetRotation(ReadRotatorFromBytes(Source, index).Quaternion());

		index += rotatorDataSize;
		transform.SetScale3D(ReadBytesToVector(Source, index));

		return transform;
	}

	UFUNCTION(BlueprintPure, Category = "BinarySerialization")
	static TArray<uint8> FloatToBytes(float value) {
		const uint8* ptr = reinterpret_cast<const uint8*>(&value);
		TArray<uint8> bytes;

		for (int i = 0; i < sizeof(float); i++) {
			bytes.Add(ptr[i]);
		}

		return bytes;
	}

	UFUNCTION(BlueprintPure, Category = "BinarySerialization")
	static TArray<uint8> Int32ToBytes(int32 value) {
		const uint8* ptr = reinterpret_cast<const uint8*>(&value);
		TArray<uint8> bytes;

		for (int i = 0; i < sizeof(int32); i++) {
			bytes.Add(ptr[i]);
		}

		return bytes;
	}

	UFUNCTION(BlueprintPure, Category = "BinarySerialization")
	static float BytesToFloat(TArray<uint8> bytes) {
		return *(reinterpret_cast<float*>(bytes.GetData()));
	}

	UFUNCTION(BlueprintPure, Category = "BinarySerialization")
	static float BytesToInt32(TArray<uint8> bytes) {
		return *(reinterpret_cast<int32*>(bytes.GetData()));
	}
};
