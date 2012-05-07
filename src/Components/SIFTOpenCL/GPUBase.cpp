

#include "GPUBase.h"


GPUBase::GPUBase(char* source, char* KernelName)
{
	kernelFuncName = KernelName;
	size_t szKernelLength = 0;
	size_t szKernelLengthFilter = 0;
	size_t szKernelLengthSum = 0;
	char* SourceOpenCLShared;
	char* SourceOpenCL;
	iBlockDimX = 16;
	iBlockDimY = 16;

	
	GPUContext = GPU::getInstance().GPUContext;
	GPUCommandQueue = GPU::getInstance().GPUCommandQueue;
	
		
    	// Load OpenCL kernel
	SourceOpenCLShared = oclLoadProgSource("/home/mati/Dropbox/MGR/DisCODe/DCL_SIFTOpenCL/src/Components/SIFTOpenCL/OpenCL/GPUCode.cl", "// My comment\n", &szKernelLength);

	SourceOpenCL = oclLoadProgSource(source, "// My comment\n", &szKernelLengthFilter);
	
	printf("oclLoadProgSource \n");
	
	cout << "Pliki:" << endl;
	cout << szKernelLength << endl;
	cout << szKernelLengthFilter << endl;
	
	
	
	szKernelLengthSum = szKernelLength + szKernelLengthFilter + 100;
	char* sourceCL = new char[szKernelLengthSum];
	
	
	strcpy(sourceCL,SourceOpenCLShared);
	strcat (sourceCL, SourceOpenCL);
	
	
	GPUProgram = clCreateProgramWithSource( GPUContext , 1, (const char **)&sourceCL, &szKernelLengthSum, &GPUError);
	CheckError(GPUError);
	
	printf("clCreateProgramWithSource \n");

	// Build the program with 'mad' Optimization option
	char *flags = "-cl-unsafe-math-optimizations";

	GPUError = clBuildProgram(GPUProgram, 0, NULL, flags, NULL, NULL);
	CheckError(GPUError);
	
	
	printf("clBuildProgram  \n");
	
	cout << kernelFuncName << endl;

	GPUKernel = clCreateKernel(GPUProgram, kernelFuncName, &GPUError);
	CheckError(GPUError);

	

}

GPUBase::GPUBase()
{
	iBlockDimX = 16;
	iBlockDimY = 16;
}

bool GPUBase::CreateBuffersIn(int maxBufferSize, int numbOfBuffers)
{
	numberOfBuffersIn = numbOfBuffers;
	buffersListIn = new cl_mem[numberOfBuffersIn];
	sizeBuffersIn = new int[numberOfBuffersIn];

	for (int i = 0; i < numberOfBuffersIn ; i++)
	{
		buffersListIn[i] = clCreateBuffer(GPUContext, CL_MEM_READ_WRITE, maxBufferSize, NULL, &GPUError);
		CheckError(GPUError);
	}

	return true;
}

int GPUBase::GetKernelSize(double sigma, double cut_off)
{
	unsigned int i;
	for (i=0;i<MAX_KERNEL_SIZE;i++)
		if (exp(-((double)(i*i))/(2.0*sigma*sigma))<cut_off)
			break;
	unsigned int size = 2*i-1;
	return size;
}


bool GPUBase::CreateBuffersOut( int maxBufferSize, int numbOfBuffers)
{
	numberOfBuffersOut = numbOfBuffers;
	buffersListOut = new cl_mem[numberOfBuffersOut];
	sizeBuffersOut = new int[numberOfBuffersOut];

	for (int i = 0; i < numberOfBuffersOut ; i++)
	{
		buffersListOut[i] = clCreateBuffer(GPUContext, CL_MEM_READ_WRITE, maxBufferSize, NULL, &GPUError);
		CheckError(GPUError);
	}
	return true;
}


void GPUBase::CheckError( int code )
{
	switch(code)
	{
	case CL_SUCCESS:
		return;
		break;
	default:
		cout << "OTHERS ERROR" << endl;
	}
}


bool GPUBase::SendImageToBuffers(IplImage* img, ... )
{
	if(buffersListIn == NULL)
		return false;

	//clock_t start, finish;
	//double duration = 0;
	//start = clock();

		imageHeight = img->height;
		imageWidth = img->width;

		sizeBuffersIn[0] = img->width*img->height*sizeof(float);
		GPUError = clEnqueueWriteBuffer(GPUCommandQueue, buffersListIn[0], CL_TRUE, 0, img->width*img->height*sizeof(float) , (void*)img->imageData, 0, NULL, NULL);
		CheckError(GPUError);

		va_list arg_ptr;
		va_start(arg_ptr, img);
	
		for(int i = 1 ; i<numberOfBuffersIn ; i++)
		{
			IplImage* tmpImg = va_arg(arg_ptr, IplImage*);
			sizeBuffersIn[i] = tmpImg->width*tmpImg->height*sizeof(float);
			GPUError = clEnqueueWriteBuffer(GPUCommandQueue, buffersListIn[i], CL_TRUE, 0, tmpImg->width*tmpImg->height*sizeof(float) , (void*)tmpImg->imageData, 0, NULL, NULL);
			CheckError(GPUError);
		}
		va_end(arg_ptr);

	//finish = clock();
	//duration = (double)(finish - start) / CLOCKS_PER_SEC;
	//cout << "SEND IMG TO GPU: " << endl;
	//cout << duration << endl;
}


bool GPUBase::ReceiveImageData( IplImage* img, ... )
{
	if(buffersListOut == NULL)
		return false;

	sizeBuffersOut[0] = img->width*img->height*sizeof(float);
	GPUError = clEnqueueReadBuffer(GPUCommandQueue, buffersListOut[0], CL_TRUE, 0, img->width*img->height*sizeof(float) , (void*)img->imageData, 0, NULL, NULL);
	CheckError(GPUError);

	va_list arg_ptr;
	va_start(arg_ptr, img);

	for(int i = 1 ; i<numberOfBuffersOut ; i++)
	{
		IplImage* tmpImg = va_arg(arg_ptr, IplImage*);
		sizeBuffersOut[i] = tmpImg->width*tmpImg->height*sizeof(float);
		GPUError = clEnqueueReadBuffer(GPUCommandQueue, buffersListOut[i], CL_TRUE, 0, tmpImg->width*tmpImg->height*sizeof(float) , (void*)tmpImg->imageData, 0, NULL, NULL);
		CheckError(GPUError);
	}
	va_end(arg_ptr);
}


size_t GPUBase::shrRoundUp(int group_size, int global_size)
{
	if(global_size < 80)
		global_size = 80;
	int r = global_size % group_size;
	if(r == 0)
	{
		return global_size;
	} else
	{
		return global_size + group_size - r;
	}
}

char* GPUBase::oclLoadProgSource(const char* cFilename, const char* cPreamble, size_t* szFinalLength)
{
	// locals
	FILE* pFileStream = NULL;
	size_t szSourceLength;


	pFileStream = fopen(cFilename, "rb");
	if(pFileStream == 0)
	{
		return NULL;
	}
	size_t szPreambleLength = strlen(cPreamble);

	// get the length of the source code
	fseek(pFileStream, 0, SEEK_END);
	szSourceLength = ftell(pFileStream);
	fseek(pFileStream, 0, SEEK_SET);

	// allocate a buffer for the source code string and read it in
	char* cSourceString = (char *)malloc(szSourceLength + szPreambleLength + 1);
	memcpy(cSourceString, cPreamble, szPreambleLength);
	if (fread((cSourceString) + szPreambleLength, szSourceLength, 1, pFileStream) != 1)
	{
		fclose(pFileStream);
		free(cSourceString);
		return 0;
	}

	// close the file and return the total length of the combined (preamble + source) string
	fclose(pFileStream);
	if(szFinalLength != 0)
	{
		*szFinalLength = szSourceLength + szPreambleLength;
	}
	cSourceString[szSourceLength + szPreambleLength] = '\0';

	return cSourceString;
}

GPUBase::~GPUBase()
{
	if(GPUCommandQueue)clReleaseCommandQueue(GPUCommandQueue);
	if(GPUContext)clReleaseContext(GPUContext);

	for(int i = 1 ; i<numberOfBuffersOut ; i++)
	{
		if(buffersListOut[i])clReleaseMemObject(buffersListOut[i]);
	}

	for(int i = 1 ; i<numberOfBuffersIn ; i++)
	{
		if(buffersListIn[i])clReleaseMemObject(buffersListIn[i]);
	}
}

















