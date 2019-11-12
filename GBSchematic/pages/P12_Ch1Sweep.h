#pragma once

struct Gameboy;

struct P12_Ch1Sweep {

  static void tick(const Gameboy& a, const Gameboy& b, Gameboy& c);

  // presumably this is the freq out
  bool AXAN,EVAB,DYGY,HOPO,HYXU,HOLU,FELY,EDUL,HAVO,JYKA,HYKA;

  bool ATYS;

private:

  //----------
  // regs

  bool DOLY,DOFY,DEXE,DELE,EXAP,FAXO,GYME,JYME,KARE,JODE,GALO;
  bool BEKU,AGEZ,ELUX,EXAC,FEDO,FUDE,JOTA,JOLU,GOGA,JEFA,FABU;
  bool DEVA,ETER,DEFA,EDOK,EPYR,GELE,JETE,JAPE,HELE,HOPA,HORA;

  //----------
  // cells

  bool DEPU,DEBY,DYLA;
  bool BYFU,BOFU,BYSU,DULO,DYLU,JULO,KOPU,ETUV,FULE,GULU,DEKE;
  bool AFEG,BUDO,BUGU,ETOL,ELER,KYPA,KOVU,GOPE,GOLO,GETA,GYLU;
  bool AJUX,AMAC,BASO,EMAR,ETOK,KYFU,KAVO,FEGA,FOKE,FOPU,EJYF;
  bool APAJ,BOVU,BOXU,ESEL,ELUF,KAJU,KAPO,GAMO,GYFU,GATO,EFOR;
  bool ARYL,BYLE;

  bool BOJO,APAT,BYRU,CYKY,DEBO,FOHY,KOVO,KEKE,HUNY,HOXE,JUTA;

  bool KEDO,JUJU,KAPE;
  bool AFYR,BUVO,AFUG,BAPU,EREG,EVOF,KEVY,KAXY,JEHY,JOCY,KOKO;
  bool BEJU,BESO,BEGE,DACE,EKEM,GOVO,KOLA,KYRY,HAWY,HOLA,HOZU;
  bool AVUF,AFUX,AGOR,BEWO,ENOK,EZUK,KYBO,KETO,HYVU,HOBU,JADO;
  bool FAJA,EJYB,CYBE,BECY;
  bool CULU,DOZY,CALE,DYME,FURE,GOLY,KEFE,HEFY,GOPO,GELA,GYLO;
};

