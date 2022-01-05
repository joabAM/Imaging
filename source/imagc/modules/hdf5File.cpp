/*
 * hdf5File.c
 *
 *  Created on: Nov 9, 2010
 *      Author: roj-idl71
 */
#include "hdf5File.h"

CHeaderImag::CHeaderImag() {
	// TODO Auto-generated constructor stub
}

CHeaderImag::~CHeaderImag() {
	// TODO Auto-generated constructor stub
}

int CHeaderImag::write2file(){

	//HEADER IMAGING FILE VARIABLES
		char		filename[140];
		char		chr_buffer[55];
		int			size[2];
		int			nheis;
		int			nrows, ncolumns;
		float		procrange[2], beaconrange[2];
		char		version[10];

		hid_t       h5file, rootgroup, group, gcpl, filetype, memtype, space, dset, attr, dtype;          /* Handles */
	    herr_t      status;
	    hsize_t     dim1D;

		sprintf(filename,"%s/head%04d%03d.h5",path,year,doy);

		nheis = (hindex[1]-hindex[0]+1);

		procrange[0] = hindex[0]*dh+h0;
		procrange[1] = hindex[1]*dh+h0;

		beaconrange[0] = bindex[0]*dh+h0;
		beaconrange[1] = bindex[1]*dh+h0;

		nrows = nprofiles*nheis;
		ncolumns = nx*ny;

		size[0] = nrows;
		size[1] = ncolumns;

	    /*
	     * Create a new file using the default properties.
	     */
	    h5file = H5Fcreate (filename, H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);

	    //File version
	    strcpy(version, VERSION);
	    version[9] = '\0';

		dtype = H5Tcopy (H5T_C_S1);
		H5Tset_size(dtype, strlen(version));

		dim1D = 1;
		space = H5Screate_simple(1, &dim1D, NULL);

		dset = H5Dcreate (h5file, "Version", dtype, space, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
		status = H5Dwrite (dset, dtype, H5S_ALL, H5S_ALL, H5P_DEFAULT, &version);

		status = H5Dclose (dset);
		status = H5Sclose (space);

		//Calibration
		space = H5Screate (H5S_SCALAR);;
		dset = H5Dcreate (h5file, "Calibration", H5T_STD_I64LE, space, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
		status = H5Dwrite (dset, H5T_NATIVE_INT, H5S_ALL, H5S_ALL, H5P_DEFAULT, &calibration);

		status = H5Dclose (dset);
		status = H5Sclose (space);

		//*******************************************************************************
	    //*******************************************************************************
	    /*
	     * metadata group
	     */

	    gcpl = H5Pcreate (H5P_LINK_CREATE);
	    status = H5Pset_create_intermediate_group (gcpl, 1);
	    rootgroup = H5Gcreate (h5file, "/metadata", gcpl, H5P_DEFAULT, H5P_DEFAULT);

		//*******************************************************************************
		//*******************************************************************************

		status = H5Pset_create_intermediate_group (gcpl, 1);
		group = H5Gcreate (h5file, "/metadata/Radar parameters", gcpl, H5P_DEFAULT, H5P_DEFAULT);

		//********IPP*********
		space = H5Screate (H5S_SCALAR);;
		dset = H5Dcreate (group, "IPP", H5T_IEEE_F32LE, space, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
		status = H5Dwrite (dset, H5T_NATIVE_FLOAT, H5S_ALL, H5S_ALL, H5P_DEFAULT, &ipp);

		status = H5Dclose (dset);
		status = H5Sclose (space);

		//********Initial height*********
		space = H5Screate (H5S_SCALAR);;
		dset = H5Dcreate (group, "H0", H5T_IEEE_F32LE, space, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
		status = H5Dwrite (dset, H5T_NATIVE_FLOAT, H5S_ALL, H5S_ALL, H5P_DEFAULT, &h0);

		status = H5Dclose (dset);
		status = H5Sclose (space);

		//********Delta height*********
		space = H5Screate (H5S_SCALAR);;
		dset = H5Dcreate (group, "Delta H", H5T_IEEE_F32LE, space, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
		status = H5Dwrite (dset, H5T_NATIVE_FLOAT, H5S_ALL, H5S_ALL, H5P_DEFAULT, &dh);

		status = H5Dclose (dset);
		status = H5Sclose (space);

		//********Number of samples*********
		space = H5Screate (H5S_SCALAR);;
		dset = H5Dcreate (group, "Number of samples", H5T_STD_I64LE, space, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
		status = H5Dwrite (dset, H5T_NATIVE_INT, H5S_ALL, H5S_ALL, H5P_DEFAULT, &nsamples);

		status = H5Dclose (dset);
		status = H5Sclose (space);

		//********Wavelength*********
		space = H5Screate (H5S_SCALAR);;
		dset = H5Dcreate (group, "Wavelength", H5T_IEEE_F32LE, space, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
		status = H5Dwrite (dset, H5T_NATIVE_FLOAT, H5S_ALL, H5S_ALL, H5P_DEFAULT, &wavelength);

		status = H5Dclose (dset);
		status = H5Sclose (space);

		status = H5Gclose (group);

		//*******************************************************************************
		//*******************************************************************************

		group = H5Gcreate (h5file, "/metadata/Processing parameters", gcpl, H5P_DEFAULT, H5P_DEFAULT);

		//Number of profiles
		space = H5Screate (H5S_SCALAR);;
		dset = H5Dcreate (group, "Number of FFT points", H5T_STD_I64LE, space, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
		status = H5Dwrite (dset, H5T_NATIVE_INT, H5S_ALL, H5S_ALL, H5P_DEFAULT, &nprofiles);

		status = H5Dclose (dset);
		status = H5Sclose (space);

		//Number of heights
		space = H5Screate (H5S_SCALAR);;
		dset = H5Dcreate (group, "Number of heights", H5T_STD_I64LE, space, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
		status = H5Dwrite (dset, H5T_NATIVE_INT, H5S_ALL, H5S_ALL, H5P_DEFAULT, &nheis);

		status = H5Dclose (dset);
		status = H5Sclose (space);

		//Number of nx points
		space = H5Screate (H5S_SCALAR);;
		dset = H5Dcreate (group, "Nx", H5T_STD_I64LE, space, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
		status = H5Dwrite (dset, H5T_NATIVE_INT, H5S_ALL, H5S_ALL, H5P_DEFAULT, &nx);

		status = H5Dclose (dset);
		status = H5Sclose (space);

		//Number of ny points
		space = H5Screate (H5S_SCALAR);;
		dset = H5Dcreate (group, "Ny", H5T_STD_I64LE, space, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
		status = H5Dwrite (dset, H5T_NATIVE_INT, H5S_ALL, H5S_ALL, H5P_DEFAULT, &ny);

		status = H5Dclose (dset);
		status = H5Sclose (space);

		dim1D = 2;

		//********Height range*********
		space = H5Screate_simple (1, &dim1D, NULL);
		dset = H5Dcreate (group, "Height range", H5T_IEEE_F32LE, space, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
		status = H5Dwrite (dset, H5T_NATIVE_FLOAT, H5S_ALL, H5S_ALL, H5P_DEFAULT, procrange);

		status = H5Dclose (dset);
		status = H5Sclose (space);

		//********Height index range*********
		space = H5Screate_simple (1, &dim1D, NULL);
		dset = H5Dcreate (group, "Height index range", H5T_STD_I64LE, space, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
		status = H5Dwrite (dset, H5T_NATIVE_INT, H5S_ALL, H5S_ALL, H5P_DEFAULT, hindex);

		status = H5Dclose (dset);
		status = H5Sclose (space);

		//********Beacon range*********
		space = H5Screate_simple (1, &dim1D, NULL);
		dset = H5Dcreate (group, "Beacon range", H5T_IEEE_F32LE, space, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
		status = H5Dwrite (dset, H5T_NATIVE_FLOAT, H5S_ALL, H5S_ALL, H5P_DEFAULT, beaconrange);

		status = H5Dclose (dset);
		status = H5Sclose (space);

		//********Beacon index range*********
		space = H5Screate_simple (1, &dim1D, NULL);
		dset = H5Dcreate (group, "Beacon index range", H5T_STD_I64LE, space, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
		status = H5Dwrite (dset, H5T_NATIVE_INT, H5S_ALL, H5S_ALL, H5P_DEFAULT, bindex);

		status = H5Dclose (dset);
		status = H5Sclose (space);

		//********dcosx*********
		space = H5Screate (H5S_SCALAR);;
		dset = H5Dcreate (group, "dcosx", H5T_IEEE_F32LE, space, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
		status = H5Dwrite (dset, H5T_NATIVE_FLOAT, H5S_ALL, H5S_ALL, H5P_DEFAULT, &scalex);

		status = H5Dclose (dset);
		status = H5Sclose (space);

		//********dcosy*********
		space = H5Screate (H5S_SCALAR);;
		dset = H5Dcreate (group, "dcosy", H5T_IEEE_F32LE, space, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
		status = H5Dwrite (dset, H5T_NATIVE_FLOAT, H5S_ALL, H5S_ALL, H5P_DEFAULT, &scaley);

		status = H5Dclose (dset);
		status = H5Sclose (space);

		//********dcosy_offset*********
		space = H5Screate (H5S_SCALAR);;
		dset = H5Dcreate (group, "dcosy offset", H5T_IEEE_F32LE, space, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
		status = H5Dwrite (dset, H5T_NATIVE_FLOAT, H5S_ALL, H5S_ALL, H5P_DEFAULT, &yoffset);

		status = H5Dclose (dset);
		status = H5Sclose (space);

		//********rotangle*********
		space = H5Screate (H5S_SCALAR);;
		dset = H5Dcreate (group, "Rotation angle", H5T_IEEE_F32LE, space, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
		status = H5Dwrite (dset, H5T_NATIVE_FLOAT, H5S_ALL, H5S_ALL, H5P_DEFAULT, &rotangle);

		status = H5Dclose (dset);
		status = H5Sclose (space);

		//Number of channels
		space = H5Screate (H5S_SCALAR);;
		dset = H5Dcreate (group, "Number of channels", H5T_STD_I64LE, space, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
		status = H5Dwrite (dset, H5T_NATIVE_INT, H5S_ALL, H5S_ALL, H5P_DEFAULT, &nchannels);

		status = H5Dclose (dset);
		status = H5Sclose (space);

		dim1D = nchannels;

		//********Hydra phase*********
		space = H5Screate_simple (1, &dim1D, NULL);
		dset = H5Dcreate (group, "Hydra phase", H5T_IEEE_F32LE, space, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
		status = H5Dwrite (dset, H5T_NATIVE_FLOAT, H5S_ALL, H5S_ALL, H5P_DEFAULT, phydraphase);

		status = H5Dclose (dset);
		status = H5Sclose (space);

		//********Delta phase*********
		space = H5Screate_simple (1, &dim1D, NULL);
		dset = H5Dcreate (group, "Delta phase", H5T_IEEE_F32LE, space, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
		status = H5Dwrite (dset, H5T_NATIVE_FLOAT, H5S_ALL, H5S_ALL, H5P_DEFAULT, pdeltaphase);

		status = H5Dclose (dset);
		status = H5Sclose (space);

	    status = H5Gclose (group);

	    //*******************************************************************************
		//*******************************************************************************
		group = H5Gcreate (h5file, "/metadata/Array dimension", gcpl, H5P_DEFAULT, H5P_DEFAULT);

		//Filetype and memtype for info
		filetype = H5Tcopy (H5T_FORTRAN_S1);
		status = H5Tset_size (filetype, 40 - 1);
		memtype = H5Tcopy (H5T_C_S1);
		status = H5Tset_size (memtype, 40);

		//********size array*********
		dim1D = 2;
		space = H5Screate_simple (1, &dim1D, NULL);
		dset = H5Dcreate (group, "Size array", H5T_STD_I64LE, space, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
		status = H5Dwrite (dset, H5T_NATIVE_INT, H5S_ALL, H5S_ALL, H5P_DEFAULT, &size);

		status = H5Dclose (dset);
		status = H5Sclose (space);

		//**********Rows*********
		space = H5Screate (H5S_SCALAR);;
		dset = H5Dcreate (group, "Rows", H5T_STD_I64LE, space, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
		status = H5Dwrite (dset, H5T_NATIVE_INT, H5S_ALL, H5S_ALL, H5P_DEFAULT, &nrows);

		status = H5Sclose (space);

		//Row attribute
		strcpy(chr_buffer,"Number of FFT Points * Number of heights\n");

		space = H5Screate (H5S_SCALAR);
		attr = H5Acreate (dset, "rows", filetype, space, H5P_DEFAULT, H5P_DEFAULT);
		status = H5Awrite (attr, memtype, chr_buffer);

		status = H5Sclose (space);
		status = H5Aclose (attr);
		status = H5Dclose (dset);
		//************************

		//***********Columns******
		space = H5Screate (H5S_SCALAR);;
		dset = H5Dcreate (group, "Columns", H5T_STD_I64LE, space, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
		status = H5Dwrite (dset, H5T_NATIVE_INT, H5S_ALL, H5S_ALL, H5P_DEFAULT, &ncolumns);

		status = H5Sclose (space);

		//Column attribute
		strcpy(chr_buffer,"Nx * Ny\n");

		space = H5Screate (H5S_SCALAR);
		attr = H5Acreate (dset, "columns", filetype, space, H5P_DEFAULT, H5P_DEFAULT);
		status = H5Awrite (attr, memtype, chr_buffer);

		status = H5Sclose (space);
		status = H5Aclose (attr);
		status = H5Dclose (dset);

		status = H5Gclose (group);

	    status = H5Pclose (gcpl);
	    status = H5Gclose (rootgroup);
	    status = H5Fclose (h5file);

	    return (int) status;

}

int svHeader2HDF5v2(char* path, int year, int doy, int hour, int min, int sec,
		float h0, float dh, int nsamples, float ipp,
		unsigned int ncoh, unsigned int nincoh,
		int nx, int ny,
		float scalex, float scaley, float rotangle, float offset,
		float wavelength,
		int heiIndex[2], int beaconIndex[2],
		int nFFTPoints,
		unsigned int nchannels, float *phydraphase, float *pdeltaphase){

	//HEADER IMAGING FILE VARIABLES
	char		filename[140];
	char		chr_buffer[55];
	int			size[2] = {0,0};
	int			nHeis;
	int			nrows, ncolumns;
	float		procrange[2] = {0,0}, beaconrange[2] = {0,0};
	char		version[10];

	hid_t       h5file, rootgroup, group, gcpl, filetype, memtype, space, dset, attr, dtype;          /* Handles */
    herr_t      status;
    hsize_t     dim1D;

	sprintf(filename,"%s/head%04d%03d.h5",path,year,doy);

	nHeis = (heiIndex[1]-heiIndex[0]+1);

	procrange[0] = heiIndex[0]*dh+h0;
	procrange[1] = heiIndex[1]*dh+h0;

	beaconrange[0] = beaconIndex[0]*dh+h0;
	beaconrange[1] = beaconIndex[1]*dh+h0;

	nrows = nFFTPoints*nHeis;
	ncolumns = nx*ny;

	size[0] = nrows;
	size[1] = ncolumns;

    /*
     * Create a new file using the default properties.
     */
    h5file = H5Fcreate (filename, H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);

    //File version
	strcpy(version, VERSION);
	version[9] = '\0';

	dtype = H5Tcopy (H5T_C_S1);
	H5Tset_size(dtype, strlen(version));

	dim1D = 1;
	space = H5Screate_simple(1, &dim1D, NULL);

	dset = H5Dcreate (h5file, "Version", dtype, space, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
	status = H5Dwrite (dset, dtype, H5S_ALL, H5S_ALL, H5P_DEFAULT, &version);

	status = H5Dclose (dset);
	status = H5Sclose (space);

	//*******************************************************************************
    //*******************************************************************************
    /*
     * metadata group
     */

    gcpl = H5Pcreate (H5P_LINK_CREATE);
    status = H5Pset_create_intermediate_group (gcpl, 1);
    rootgroup = H5Gcreate (h5file, "/metadata", gcpl, H5P_DEFAULT, H5P_DEFAULT);

	//*******************************************************************************
	//*******************************************************************************
	group = H5Gcreate (h5file, "/metadata/Array dimension", gcpl, H5P_DEFAULT, H5P_DEFAULT);

	//Filetype and memtype for info
	filetype = H5Tcopy (H5T_FORTRAN_S1);
	status = H5Tset_size (filetype, 40 - 1);
	memtype = H5Tcopy (H5T_C_S1);
	status = H5Tset_size (memtype, 40);

	dim1D = 2;

	//********size array*********
	space = H5Screate_simple (1, &dim1D, NULL);
	dset = H5Dcreate (group, "Size array", H5T_STD_I64LE, space, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
	status = H5Dwrite (dset, H5T_NATIVE_INT, H5S_ALL, H5S_ALL, H5P_DEFAULT, &size);

	status = H5Dclose (dset);
	status = H5Sclose (space);

	//**********Rows*********
	space = H5Screate (H5S_SCALAR);;
	dset = H5Dcreate (group, "Rows", H5T_STD_I64LE, space, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
	status = H5Dwrite (dset, H5T_NATIVE_INT, H5S_ALL, H5S_ALL, H5P_DEFAULT, &nrows);

	status = H5Sclose (space);

	//Row attribute
	strcpy(chr_buffer,"Number of FFT Points * Number of heights\n");

	space = H5Screate (H5S_SCALAR);
	attr = H5Acreate (dset, "rows", filetype, space, H5P_DEFAULT, H5P_DEFAULT);
	status = H5Awrite (attr, memtype, chr_buffer);

	status = H5Sclose (space);
	status = H5Aclose (attr);
	status = H5Dclose (dset);
	//************************

	//***********Columns******
	space = H5Screate (H5S_SCALAR);;
	dset = H5Dcreate (group, "Columns", H5T_STD_I64LE, space, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
	status = H5Dwrite (dset, H5T_NATIVE_INT, H5S_ALL, H5S_ALL, H5P_DEFAULT, &ncolumns);

	status = H5Sclose (space);

	//Column attribute
	strcpy(chr_buffer,"Nx * Ny\n");

	space = H5Screate (H5S_SCALAR);
	attr = H5Acreate (dset, "columns", filetype, space, H5P_DEFAULT, H5P_DEFAULT);
	status = H5Awrite (attr, memtype, chr_buffer);

	status = H5Sclose (space);
	status = H5Aclose (attr);
	status = H5Dclose (dset);

	status = H5Gclose (group);

	//*******************************************************************************
	//*******************************************************************************

	group = H5Gcreate (h5file, "/metadata/Processing parameters", gcpl, H5P_DEFAULT, H5P_DEFAULT);

	//Number of Coherent Integrations
	space = H5Screate (H5S_SCALAR);;
	dset = H5Dcreate (group, "NCOH", H5T_STD_I64LE, space, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
	status = H5Dwrite (dset, H5T_NATIVE_INT, H5S_ALL, H5S_ALL, H5P_DEFAULT, &ncoh);

	//NCOH attribute
	strcpy(chr_buffer,"Number of Coherent Integrations\n");

	attr = H5Acreate (dset, "ncoh", filetype, space, H5P_DEFAULT, H5P_DEFAULT);
	status = H5Awrite (attr, memtype, chr_buffer);
	status = H5Aclose (attr);

	status = H5Dclose (dset);
	status = H5Sclose (space);

	//Number of Coherent Integrations
	space = H5Screate (H5S_SCALAR);;
	dset = H5Dcreate (group, "NINCOH", H5T_STD_I64LE, space, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
	status = H5Dwrite (dset, H5T_NATIVE_INT, H5S_ALL, H5S_ALL, H5P_DEFAULT, &nincoh);

	//NCOH attribute
	strcpy(chr_buffer,"Number of Incoherent Integrations\n");

	attr = H5Acreate (dset, "nincoh", filetype, space, H5P_DEFAULT, H5P_DEFAULT);
	status = H5Awrite (attr, memtype, chr_buffer);
	status = H5Aclose (attr);

	status = H5Dclose (dset);
	status = H5Sclose (space);

	//Number of profiles
	space = H5Screate (H5S_SCALAR);;
	dset = H5Dcreate (group, "Number of FFT points", H5T_STD_I64LE, space, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
	status = H5Dwrite (dset, H5T_NATIVE_INT, H5S_ALL, H5S_ALL, H5P_DEFAULT, &nFFTPoints);

	status = H5Dclose (dset);
	status = H5Sclose (space);

	//Number of heights
	space = H5Screate (H5S_SCALAR);;
	dset = H5Dcreate (group, "Number of heights", H5T_STD_I64LE, space, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
	status = H5Dwrite (dset, H5T_NATIVE_INT, H5S_ALL, H5S_ALL, H5P_DEFAULT, &nHeis);

	status = H5Dclose (dset);
	status = H5Sclose (space);

	//Number of nx points
	space = H5Screate (H5S_SCALAR);;
	dset = H5Dcreate (group, "Nx", H5T_STD_I64LE, space, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
	status = H5Dwrite (dset, H5T_NATIVE_INT, H5S_ALL, H5S_ALL, H5P_DEFAULT, &nx);

	status = H5Dclose (dset);
	status = H5Sclose (space);

	//Number of ny points
	space = H5Screate (H5S_SCALAR);;
	dset = H5Dcreate (group, "Ny", H5T_STD_I64LE, space, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
	status = H5Dwrite (dset, H5T_NATIVE_INT, H5S_ALL, H5S_ALL, H5P_DEFAULT, &ny);

	status = H5Dclose (dset);
	status = H5Sclose (space);

	//********Height range*********
	space = H5Screate_simple (1, &dim1D, NULL);
	dset = H5Dcreate (group, "Height range", H5T_IEEE_F32LE, space, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
	status = H5Dwrite (dset, H5T_NATIVE_FLOAT, H5S_ALL, H5S_ALL, H5P_DEFAULT, procrange);

	status = H5Dclose (dset);
	status = H5Sclose (space);

	//********Height index range*********
	space = H5Screate_simple (1, &dim1D, NULL);
	dset = H5Dcreate (group, "Height index range", H5T_STD_I64LE, space, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
	status = H5Dwrite (dset, H5T_NATIVE_INT, H5S_ALL, H5S_ALL, H5P_DEFAULT, heiIndex);

	status = H5Dclose (dset);
	status = H5Sclose (space);

	//********Beacon range*********
	space = H5Screate_simple (1, &dim1D, NULL);
	dset = H5Dcreate (group, "Beacon range", H5T_IEEE_F32LE, space, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
	status = H5Dwrite (dset, H5T_NATIVE_FLOAT, H5S_ALL, H5S_ALL, H5P_DEFAULT, beaconrange);

	status = H5Dclose (dset);
	status = H5Sclose (space);

	//********Beacon index range*********
	space = H5Screate_simple (1, &dim1D, NULL);
	dset = H5Dcreate (group, "Beacon index range", H5T_STD_I64LE, space, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
	status = H5Dwrite (dset, H5T_NATIVE_INT, H5S_ALL, H5S_ALL, H5P_DEFAULT, beaconIndex);

	status = H5Dclose (dset);
	status = H5Sclose (space);

	//********dcosx*********
	space = H5Screate (H5S_SCALAR);;
	dset = H5Dcreate (group, "dcosx", H5T_IEEE_F32LE, space, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
	status = H5Dwrite (dset, H5T_NATIVE_FLOAT, H5S_ALL, H5S_ALL, H5P_DEFAULT, &scalex);

	status = H5Dclose (dset);
	status = H5Sclose (space);

	//********dcosy*********
	space = H5Screate (H5S_SCALAR);;
	dset = H5Dcreate (group, "dcosy", H5T_IEEE_F32LE, space, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
	status = H5Dwrite (dset, H5T_NATIVE_FLOAT, H5S_ALL, H5S_ALL, H5P_DEFAULT, &scaley);

	status = H5Dclose (dset);
	status = H5Sclose (space);

	//********dcosy_offset*********
	space = H5Screate (H5S_SCALAR);;
	dset = H5Dcreate (group, "dcosy offset", H5T_IEEE_F32LE, space, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
	status = H5Dwrite (dset, H5T_NATIVE_FLOAT, H5S_ALL, H5S_ALL, H5P_DEFAULT, &offset);

	status = H5Dclose (dset);
	status = H5Sclose (space);

	//********rotangle*********
	space = H5Screate (H5S_SCALAR);;
	dset = H5Dcreate (group, "Rotation angle", H5T_IEEE_F32LE, space, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
	status = H5Dwrite (dset, H5T_NATIVE_FLOAT, H5S_ALL, H5S_ALL, H5P_DEFAULT, &rotangle);

	status = H5Dclose (dset);
	status = H5Sclose (space);

	//Number of channels
	space = H5Screate (H5S_SCALAR);;
	dset = H5Dcreate (group, "Number of channels", H5T_STD_I64LE, space, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
	status = H5Dwrite (dset, H5T_NATIVE_INT, H5S_ALL, H5S_ALL, H5P_DEFAULT, &nchannels);

	status = H5Dclose (dset);
	status = H5Sclose (space);

	dim1D = nchannels;

	//********Hydra phase*********
	space = H5Screate_simple (1, &dim1D, NULL);
	dset = H5Dcreate (group, "Hydra phase", H5T_IEEE_F32LE, space, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
	status = H5Dwrite (dset, H5T_NATIVE_FLOAT, H5S_ALL, H5S_ALL, H5P_DEFAULT, phydraphase);

	status = H5Dclose (dset);
	status = H5Sclose (space);

	//********Delta phase*********
	space = H5Screate_simple (1, &dim1D, NULL);
	dset = H5Dcreate (group, "Delta phase", H5T_IEEE_F32LE, space, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
	status = H5Dwrite (dset, H5T_NATIVE_FLOAT, H5S_ALL, H5S_ALL, H5P_DEFAULT, pdeltaphase);

	status = H5Dclose (dset);
	status = H5Sclose (space);

    status = H5Gclose (group);

    //*******************************************************************************
    //*******************************************************************************

    status = H5Pset_create_intermediate_group (gcpl, 1);
    group = H5Gcreate (h5file, "/metadata/Radar parameters", gcpl, H5P_DEFAULT, H5P_DEFAULT);

    //********IPP*********
	space = H5Screate (H5S_SCALAR);;
	dset = H5Dcreate (group, "IPP", H5T_IEEE_F32LE, space, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
	status = H5Dwrite (dset, H5T_NATIVE_FLOAT, H5S_ALL, H5S_ALL, H5P_DEFAULT, &ipp);

	status = H5Dclose (dset);
	status = H5Sclose (space);

    //********Initial height*********
	space = H5Screate (H5S_SCALAR);;
	dset = H5Dcreate (group, "H0", H5T_IEEE_F32LE, space, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
	status = H5Dwrite (dset, H5T_NATIVE_FLOAT, H5S_ALL, H5S_ALL, H5P_DEFAULT, &h0);

	status = H5Dclose (dset);
	status = H5Sclose (space);

    //********Delta height*********
	space = H5Screate (H5S_SCALAR);;
	dset = H5Dcreate (group, "Delta H", H5T_IEEE_F32LE, space, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
	status = H5Dwrite (dset, H5T_NATIVE_FLOAT, H5S_ALL, H5S_ALL, H5P_DEFAULT, &dh);

	status = H5Dclose (dset);
	status = H5Sclose (space);

    //********Number of samples*********
	space = H5Screate (H5S_SCALAR);;
	dset = H5Dcreate (group, "Number of samples", H5T_STD_I64LE, space, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
	status = H5Dwrite (dset, H5T_NATIVE_INT, H5S_ALL, H5S_ALL, H5P_DEFAULT, &nsamples);

	status = H5Dclose (dset);
	status = H5Sclose (space);

    //********Wavelength*********
	space = H5Screate (H5S_SCALAR);;
	dset = H5Dcreate (group, "Wavelength", H5T_IEEE_F32LE, space, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
	status = H5Dwrite (dset, H5T_NATIVE_FLOAT, H5S_ALL, H5S_ALL, H5P_DEFAULT, &wavelength);

	status = H5Dclose (dset);
	status = H5Sclose (space);

	status = H5Gclose (group);


    status = H5Pclose (gcpl);
    status = H5Gclose (rootgroup);
    status = H5Fclose (h5file);

    return (int) status;
}

int svArray2HDF5v2(char* path, char* file, float** array, int nFFTPoints, int nHeis, int nx, int ny){
	char 	filename[150];
	int		inc=1, m=0;
	char	chr_buffer[50];
	float	**wdata;				         /* Write buffer */
	char	version[10];
	int 	*index, size[2], filteredsize[2];
	int		nFilteredData=0;
	int		nrows, ncolumns;

	hid_t       h5file, group, gcpl, filetype, memtype, space, dset, attr, dtype;          /* Handles */
    herr_t      status;
    hsize_t     dim1D, dim2DFiltered[2];

    nrows = nFFTPoints*nHeis;
    ncolumns = nx*ny;

	index = new int[nrows];
	memset(index,-1,nrows*sizeof(int));

	//Eliminar las filas con suma menor a NaN*ncolumns, NaN = 0 para nuestro caso
	for(int m=0;m<nrows;m++)
		if(sasum_(&ncolumns, array[m], &inc)!=0)
		{
			index[nFilteredData] = m;
			nFilteredData++;
		}

	if(nFilteredData<1){
		delete [] index;
		//printf("\nSkip saving file nFilteredData<1\n");
		return 0;
	}

	//allowing memory to the buffer
	wdata = new float*[nFilteredData];
	wdata[0] = new float[nFilteredData*ncolumns];

	for(m=1;m<nFilteredData;m++)
		wdata[m] = wdata[m-1] + ncolumns;

	//Copy valid data to the buffer
	for(m=0;m<nFilteredData;m++)
		memcpy(wdata[m], array[index[m]], ncolumns*sizeof(float));

	size[0] = nrows;
	size[1] = ncolumns;

	filteredsize[0] = nFilteredData;
	filteredsize[1] = ncolumns;

	dim2DFiltered[0] = nFilteredData;
	dim2DFiltered[1] = ncolumns;

	//printf("Writing file: %s\n",filename);

	strcpy(filename,path);
	//strcat(filename,"/test_");
	strcat(filename,file);
	strcat(filename,".h5");

    /*
     * Create a new file using the default properties.
     */
    h5file = H5Fcreate (filename, H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);

    //File version
	strcpy(version, VERSION);
	version[9] = '\0';

	dtype = H5Tcopy (H5T_C_S1);
	H5Tset_size(dtype, strlen(version));

	dim1D = 1;
	space = H5Screate_simple(1, &dim1D, NULL);

	dset = H5Dcreate (h5file, "Version", dtype, space, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
	status = H5Dwrite (dset, dtype, H5S_ALL, H5S_ALL, H5P_DEFAULT, &version);

	status = H5Dclose (dset);
	status = H5Sclose (space);

    /*
     * metadata group
     */

    gcpl = H5Pcreate (H5P_LINK_CREATE);
    status = H5Pset_create_intermediate_group (gcpl, 1);
    group = H5Gcreate (h5file, "/metadata/", gcpl, H5P_DEFAULT, H5P_DEFAULT);

    //Size of filtered array
	dim1D = 2;
	space = H5Screate_simple (1, &dim1D, NULL);
	dset = H5Dcreate (group, "Size of filtered array", H5T_STD_I64LE, space, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
	status = H5Dwrite (dset, H5T_NATIVE_INT, H5S_ALL, H5S_ALL, H5P_DEFAULT, filteredsize);

	status = H5Dclose (dset);
	status = H5Sclose (space);

    //Row index
	dim1D = nFilteredData;
    space = H5Screate_simple (1, &dim1D, NULL);
    dset = H5Dcreate (group, "Row index array", H5T_STD_I64LE, space, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
    status = H5Dwrite (dset, H5T_NATIVE_INT, H5S_ALL, H5S_ALL, H5P_DEFAULT, index);

    status = H5Dclose (dset);
    status = H5Sclose (space);

    status = H5Gclose (group);

    group = H5Gcreate (h5file, "/metadata/Original Array dimensions", gcpl, H5P_DEFAULT, H5P_DEFAULT);

    //Original size array
    dim1D = 2;
    space = H5Screate_simple (1, &dim1D, NULL);
	dset = H5Dcreate (group, "Size array", H5T_STD_I64LE, space, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
	status = H5Dwrite (dset, H5T_NATIVE_INT, H5S_ALL, H5S_ALL, H5P_DEFAULT, size);

	status = H5Dclose (dset);
	status = H5Sclose (space);

	//Filetype and memtype for info
	filetype = H5Tcopy (H5T_FORTRAN_S1);
	status = H5Tset_size (filetype, 45);

	memtype = H5Tcopy (H5T_C_S1);
	status = H5Tset_size (memtype, 45);

    //**********Rows*********
    space = H5Screate (H5S_SCALAR);;
	dset = H5Dcreate (group, "Rows", H5T_STD_I64LE, space, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
	status = H5Dwrite (dset, H5T_NATIVE_INT, H5S_ALL, H5S_ALL, H5P_DEFAULT, &nrows);

	status = H5Sclose (space);

	//Row attribute
	strcpy(chr_buffer,"Number of FFT Points * Number of heights\n");

    space = H5Screate (H5S_SCALAR);
    attr = H5Acreate (dset, "rows", filetype, space, H5P_DEFAULT, H5P_DEFAULT);
    status = H5Awrite (attr, memtype, chr_buffer);

	status = H5Sclose (space);
	status = H5Aclose (attr);
	status = H5Dclose (dset);
	//************************

    //***********Columns******
    space = H5Screate (H5S_SCALAR);;
	dset = H5Dcreate (group, "Columns", H5T_STD_I64LE, space, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
	status = H5Dwrite (dset, H5T_NATIVE_INT, H5S_ALL, H5S_ALL, H5P_DEFAULT, &ncolumns);

	status = H5Sclose (space);

	//Column attribute
	strcpy(chr_buffer,"Nx * Ny\n");

	space = H5Screate (H5S_SCALAR);
	attr = H5Acreate (dset, "columns", filetype, space, H5P_DEFAULT, H5P_DEFAULT);
	status = H5Awrite (attr, memtype, chr_buffer);

	status = H5Sclose (space);
	status = H5Aclose (attr);
    status = H5Dclose (dset);

    //*************************

	status = H5Tclose (filetype);
	status = H5Tclose (memtype);

	status = H5Gclose (group);

	group = H5Gcreate (h5file, "/metadata/Experiment parameters", gcpl, H5P_DEFAULT, H5P_DEFAULT);

	//Number of profiles
	space = H5Screate (H5S_SCALAR);;
	dset = H5Dcreate (group, "Number of FFTs", H5T_STD_I64LE, space, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
	status = H5Dwrite (dset, H5T_NATIVE_INT, H5S_ALL, H5S_ALL, H5P_DEFAULT, &nFFTPoints);

	status = H5Dclose (dset);
	status = H5Sclose (space);

	//Number of heights
	space = H5Screate (H5S_SCALAR);;
	dset = H5Dcreate (group, "Number of Heights", H5T_STD_I64LE, space, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
	status = H5Dwrite (dset, H5T_NATIVE_INT, H5S_ALL, H5S_ALL, H5P_DEFAULT, &nHeis);

	status = H5Dclose (dset);
	status = H5Sclose (space);

	//Number of nx points
	space = H5Screate (H5S_SCALAR);;
	dset = H5Dcreate (group, "Nx", H5T_STD_I64LE, space, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
	status = H5Dwrite (dset, H5T_NATIVE_INT, H5S_ALL, H5S_ALL, H5P_DEFAULT, &nx);

	status = H5Dclose (dset);
	status = H5Sclose (space);

	//Number of ny points
	space = H5Screate (H5S_SCALAR);;
	dset = H5Dcreate (group, "Ny", H5T_STD_I64LE, space, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
	status = H5Dwrite (dset, H5T_NATIVE_INT, H5S_ALL, H5S_ALL, H5P_DEFAULT, &ny);

	status = H5Dclose (dset);
	status = H5Sclose (space);

    status = H5Pclose (gcpl);
    status = H5Gclose (group);


    /*
     * data group
     */

    gcpl = H5Pcreate (H5P_LINK_CREATE);
    status = H5Pset_create_intermediate_group (gcpl, 1);

    group = H5Gcreate (h5file, "/data", gcpl, H5P_DEFAULT, H5P_DEFAULT);

    /*
     * dataset object
     */
    space = H5Screate_simple (2, dim2DFiltered, NULL);
    dset = H5Dcreate (group, "image", H5T_IEEE_F32LE, space, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
    status = H5Dwrite (dset, H5T_NATIVE_FLOAT, H5S_ALL, H5S_ALL, H5P_DEFAULT, wdata[0]);

    status = H5Dclose (dset);
    status = H5Sclose (space);

    /*
     * Close and release resources.
     */
    status = H5Pclose (gcpl);
    status = H5Gclose (group);
    status = H5Fclose (h5file);

    delete [] wdata[0];
    delete [] wdata;
    delete [] index;

    return (int) status;
}
