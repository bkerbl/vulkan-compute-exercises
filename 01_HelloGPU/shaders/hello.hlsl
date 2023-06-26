void main(uint3 id : SV_DispatchThreadID) 
{
	printf("Hello from thread: %d\n", id.x);
}